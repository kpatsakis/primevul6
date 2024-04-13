g_markup_parse_context_parse (GMarkupParseContext  *context,
                              const gchar          *text,
                              gssize                text_len,
                              GError              **error)
{
  g_return_val_if_fail (context != NULL, FALSE);
  g_return_val_if_fail (text != NULL, FALSE);
  g_return_val_if_fail (context->state != STATE_ERROR, FALSE);
  g_return_val_if_fail (!context->parsing, FALSE);

  if (text_len < 0)
    text_len = strlen (text);

  if (text_len == 0)
    return TRUE;

  context->parsing = TRUE;


  context->current_text = text;
  context->current_text_len = text_len;
  context->current_text_end = context->current_text + text_len;
  context->iter = context->current_text;
  context->start = context->iter;

  while (context->iter != context->current_text_end)
    {
      switch (context->state)
        {
        case STATE_START:
          /* Possible next state: AFTER_OPEN_ANGLE */

          g_assert (context->tag_stack == NULL);

          /* whitespace is ignored outside of any elements */
          skip_spaces (context);

          if (context->iter != context->current_text_end)
            {
              if (*context->iter == '<')
                {
                  /* Move after the open angle */
                  advance_char (context);

                  context->state = STATE_AFTER_OPEN_ANGLE;

                  /* this could start a passthrough */
                  context->start = context->iter;

                  /* document is now non-empty */
                  context->document_empty = FALSE;
                }
              else
                {
                  set_error_literal (context,
                                     error,
                                     G_MARKUP_ERROR_PARSE,
                                     _("Document must begin with an element (e.g. <book>)"));
                }
            }
          break;

        case STATE_AFTER_OPEN_ANGLE:
          /* Possible next states: INSIDE_OPEN_TAG_NAME,
           *  AFTER_CLOSE_TAG_SLASH, INSIDE_PASSTHROUGH
           */
          if (*context->iter == '?' ||
              *context->iter == '!')
            {
              /* include < in the passthrough */
              const gchar *openangle = "<";
              add_to_partial (context, openangle, openangle + 1);
              context->start = context->iter;
              context->balance = 1;
              context->state = STATE_INSIDE_PASSTHROUGH;
            }
          else if (*context->iter == '/')
            {
              /* move after it */
              advance_char (context);

              context->state = STATE_AFTER_CLOSE_TAG_SLASH;
            }
          else if (!IS_COMMON_NAME_END_CHAR (*(context->iter)))
            {
              context->state = STATE_INSIDE_OPEN_TAG_NAME;

              /* start of tag name */
              context->start = context->iter;
            }
          else
            {
              gchar buf[8];

              set_error (context,
                         error,
                         G_MARKUP_ERROR_PARSE,
                         _("“%s” is not a valid character following "
                           "a “<” character; it may not begin an "
                           "element name"),
                         utf8_str (context->iter, buf));
            }
          break;

          /* The AFTER_CLOSE_ANGLE state is actually sort of
           * broken, because it doesn't correspond to a range
           * of characters in the input stream as the others do,
           * and thus makes things harder to conceptualize
           */
        case STATE_AFTER_CLOSE_ANGLE:
          /* Possible next states: INSIDE_TEXT, STATE_START */
          if (context->tag_stack == NULL)
            {
              context->start = NULL;
              context->state = STATE_START;
            }
          else
            {
              context->start = context->iter;
              context->state = STATE_INSIDE_TEXT;
            }
          break;

        case STATE_AFTER_ELISION_SLASH:
          /* Possible next state: AFTER_CLOSE_ANGLE */
          if (*context->iter == '>')
            {
              /* move after the close angle */
              advance_char (context);
              context->state = STATE_AFTER_CLOSE_ANGLE;
              emit_end_element (context, error);
            }
          else
            {
              gchar buf[8];

              set_error (context,
                         error,
                         G_MARKUP_ERROR_PARSE,
                         _("Odd character “%s”, expected a “>” character "
                           "to end the empty-element tag “%s”"),
                         utf8_str (context->iter, buf),
                         current_element (context));
            }
          break;

        case STATE_INSIDE_OPEN_TAG_NAME:
          /* Possible next states: BETWEEN_ATTRIBUTES */

          /* if there's a partial chunk then it's the first part of the
           * tag name. If there's a context->start then it's the start
           * of the tag name in current_text, the partial chunk goes
           * before that start though.
           */
          advance_to_name_end (context);

          if (context->iter == context->current_text_end)
            {
              /* The name hasn't necessarily ended. Merge with
               * partial chunk, leave state unchanged.
               */
              add_to_partial (context, context->start, context->iter);
            }
          else
            {
              /* The name has ended. Combine it with the partial chunk
               * if any; push it on the stack; enter next state.
               */
              add_to_partial (context, context->start, context->iter);
              push_partial_as_tag (context);

              context->state = STATE_BETWEEN_ATTRIBUTES;
              context->start = NULL;
            }
          break;

        case STATE_INSIDE_ATTRIBUTE_NAME:
          /* Possible next states: AFTER_ATTRIBUTE_NAME */

          advance_to_name_end (context);
          add_to_partial (context, context->start, context->iter);

          /* read the full name, if we enter the equals sign state
           * then add the attribute to the list (without the value),
           * otherwise store a partial chunk to be prepended later.
           */
          if (context->iter != context->current_text_end)
            context->state = STATE_AFTER_ATTRIBUTE_NAME;
          break;

        case STATE_AFTER_ATTRIBUTE_NAME:
          /* Possible next states: AFTER_ATTRIBUTE_EQUALS_SIGN */

          skip_spaces (context);

          if (context->iter != context->current_text_end)
            {
              /* The name has ended. Combine it with the partial chunk
               * if any; push it on the stack; enter next state.
               */
              if (!name_validate (context, context->partial_chunk->str, error))
                break;

              add_attribute (context, context->partial_chunk);

              context->partial_chunk = NULL;
              context->start = NULL;

              if (*context->iter == '=')
                {
                  advance_char (context);
                  context->state = STATE_AFTER_ATTRIBUTE_EQUALS_SIGN;
                }
              else
                {
                  gchar buf[8];

                  set_error (context,
                             error,
                             G_MARKUP_ERROR_PARSE,
                             _("Odd character “%s”, expected a “=” after "
                               "attribute name “%s” of element “%s”"),
                             utf8_str (context->iter, buf),
                             current_attribute (context),
                             current_element (context));

                }
            }
          break;

        case STATE_BETWEEN_ATTRIBUTES:
          /* Possible next states: AFTER_CLOSE_ANGLE,
           * AFTER_ELISION_SLASH, INSIDE_ATTRIBUTE_NAME
           */
          skip_spaces (context);

          if (context->iter != context->current_text_end)
            {
              if (*context->iter == '/')
                {
                  advance_char (context);
                  context->state = STATE_AFTER_ELISION_SLASH;
                }
              else if (*context->iter == '>')
                {
                  advance_char (context);
                  context->state = STATE_AFTER_CLOSE_ANGLE;
                }
              else if (!IS_COMMON_NAME_END_CHAR (*(context->iter)))
                {
                  context->state = STATE_INSIDE_ATTRIBUTE_NAME;
                  /* start of attribute name */
                  context->start = context->iter;
                }
              else
                {
                  gchar buf[8];

                  set_error (context,
                             error,
                             G_MARKUP_ERROR_PARSE,
                             _("Odd character “%s”, expected a “>” or “/” "
                               "character to end the start tag of "
                               "element “%s”, or optionally an attribute; "
                               "perhaps you used an invalid character in "
                               "an attribute name"),
                             utf8_str (context->iter, buf),
                             current_element (context));
                }

              /* If we're done with attributes, invoke
               * the start_element callback
               */
              if (context->state == STATE_AFTER_ELISION_SLASH ||
                  context->state == STATE_AFTER_CLOSE_ANGLE)
                emit_start_element (context, error);
            }
          break;

        case STATE_AFTER_ATTRIBUTE_EQUALS_SIGN:
          /* Possible next state: INSIDE_ATTRIBUTE_VALUE_[SQ/DQ] */

          skip_spaces (context);

          if (context->iter != context->current_text_end)
            {
              if (*context->iter == '"')
                {
                  advance_char (context);
                  context->state = STATE_INSIDE_ATTRIBUTE_VALUE_DQ;
                  context->start = context->iter;
                }
              else if (*context->iter == '\'')
                {
                  advance_char (context);
                  context->state = STATE_INSIDE_ATTRIBUTE_VALUE_SQ;
                  context->start = context->iter;
                }
              else
                {
                  gchar buf[8];

                  set_error (context,
                             error,
                             G_MARKUP_ERROR_PARSE,
                             _("Odd character “%s”, expected an open quote mark "
                               "after the equals sign when giving value for "
                               "attribute “%s” of element “%s”"),
                             utf8_str (context->iter, buf),
                             current_attribute (context),
                             current_element (context));
                }
            }
          break;

        case STATE_INSIDE_ATTRIBUTE_VALUE_SQ:
        case STATE_INSIDE_ATTRIBUTE_VALUE_DQ:
          /* Possible next states: BETWEEN_ATTRIBUTES */
          {
            gchar delim;

            if (context->state == STATE_INSIDE_ATTRIBUTE_VALUE_SQ)
              {
                delim = '\'';
              }
            else
              {
                delim = '"';
              }

            do
              {
                if (*context->iter == delim)
                  break;
              }
            while (advance_char (context));
          }
          if (context->iter == context->current_text_end)
            {
              /* The value hasn't necessarily ended. Merge with
               * partial chunk, leave state unchanged.
               */
              add_to_partial (context, context->start, context->iter);
            }
          else
            {
              gboolean is_ascii;
              /* The value has ended at the quote mark. Combine it
               * with the partial chunk if any; set it for the current
               * attribute.
               */
              add_to_partial (context, context->start, context->iter);

              g_assert (context->cur_attr >= 0);

              if (unescape_gstring_inplace (context, context->partial_chunk, &is_ascii, error) &&
                  (is_ascii || text_validate (context, context->partial_chunk->str,
                                              context->partial_chunk->len, error)))
                {
                  /* success, advance past quote and set state. */
                  context->attr_values[context->cur_attr] = context->partial_chunk;
                  context->partial_chunk = NULL;
                  advance_char (context);
                  context->state = STATE_BETWEEN_ATTRIBUTES;
                  context->start = NULL;
                }

              truncate_partial (context);
            }
          break;

        case STATE_INSIDE_TEXT:
          /* Possible next states: AFTER_OPEN_ANGLE */
          do
            {
              if (*context->iter == '<')
                break;
            }
          while (advance_char (context));

          /* The text hasn't necessarily ended. Merge with
           * partial chunk, leave state unchanged.
           */

          add_to_partial (context, context->start, context->iter);

          if (context->iter != context->current_text_end)
            {
              gboolean is_ascii;

              /* The text has ended at the open angle. Call the text
               * callback.
               */
              if (unescape_gstring_inplace (context, context->partial_chunk, &is_ascii, error) &&
                  (is_ascii || text_validate (context, context->partial_chunk->str,
                                              context->partial_chunk->len, error)))
                {
                  GError *tmp_error = NULL;

                  if (context->parser->text)
                    (*context->parser->text) (context,
                                              context->partial_chunk->str,
                                              context->partial_chunk->len,
                                              context->user_data,
                                              &tmp_error);

                  if (tmp_error == NULL)
                    {
                      /* advance past open angle and set state. */
                      advance_char (context);
                      context->state = STATE_AFTER_OPEN_ANGLE;
                      /* could begin a passthrough */
                      context->start = context->iter;
                    }
                  else
                    propagate_error (context, error, tmp_error);
                }

              truncate_partial (context);
            }
          break;

        case STATE_AFTER_CLOSE_TAG_SLASH:
          /* Possible next state: INSIDE_CLOSE_TAG_NAME */
          if (!IS_COMMON_NAME_END_CHAR (*(context->iter)))
            {
              context->state = STATE_INSIDE_CLOSE_TAG_NAME;

              /* start of tag name */
              context->start = context->iter;
            }
          else
            {
              gchar buf[8];

              set_error (context,
                         error,
                         G_MARKUP_ERROR_PARSE,
                         _("“%s” is not a valid character following "
                           "the characters “</”; “%s” may not begin an "
                           "element name"),
                         utf8_str (context->iter, buf),
                         utf8_str (context->iter, buf));
            }
          break;

        case STATE_INSIDE_CLOSE_TAG_NAME:
          /* Possible next state: AFTER_CLOSE_TAG_NAME */
          advance_to_name_end (context);
          add_to_partial (context, context->start, context->iter);

          if (context->iter != context->current_text_end)
            context->state = STATE_AFTER_CLOSE_TAG_NAME;
          break;

        case STATE_AFTER_CLOSE_TAG_NAME:
          /* Possible next state: AFTER_CLOSE_TAG_SLASH */

          skip_spaces (context);

          if (context->iter != context->current_text_end)
            {
              GString *close_name;

              close_name = context->partial_chunk;
              context->partial_chunk = NULL;

              if (*context->iter != '>')
                {
                  gchar buf[8];

                  set_error (context,
                             error,
                             G_MARKUP_ERROR_PARSE,
                             _("“%s” is not a valid character following "
                               "the close element name “%s”; the allowed "
                               "character is “>”"),
                             utf8_str (context->iter, buf),
                             close_name->str);
                }
              else if (context->tag_stack == NULL)
                {
                  set_error (context,
                             error,
                             G_MARKUP_ERROR_PARSE,
                             _("Element “%s” was closed, no element "
                               "is currently open"),
                             close_name->str);
                }
              else if (strcmp (close_name->str, current_element (context)) != 0)
                {
                  set_error (context,
                             error,
                             G_MARKUP_ERROR_PARSE,
                             _("Element “%s” was closed, but the currently "
                               "open element is “%s”"),
                             close_name->str,
                             current_element (context));
                }
              else
                {
                  advance_char (context);
                  context->state = STATE_AFTER_CLOSE_ANGLE;
                  context->start = NULL;

                  emit_end_element (context, error);
                }
              context->partial_chunk = close_name;
              truncate_partial (context);
            }
          break;

        case STATE_INSIDE_PASSTHROUGH:
          /* Possible next state: AFTER_CLOSE_ANGLE */
          do
            {
              if (*context->iter == '<')
                context->balance++;
              if (*context->iter == '>')
                {
                  gchar *str;
                  gsize len;

                  context->balance--;
                  add_to_partial (context, context->start, context->iter);
                  context->start = context->iter;

                  str = context->partial_chunk->str;
                  len = context->partial_chunk->len;

                  if (str[1] == '?' && str[len - 1] == '?')
                    break;
                  if (strncmp (str, "<!--", 4) == 0 &&
                      strcmp (str + len - 2, "--") == 0)
                    break;
                  if (strncmp (str, "<![CDATA[", 9) == 0 &&
                      strcmp (str + len - 2, "]]") == 0)
                    break;
                  if (strncmp (str, "<!DOCTYPE", 9) == 0 &&
                      context->balance == 0)
                    break;
                }
            }
          while (advance_char (context));

          if (context->iter == context->current_text_end)
            {
              /* The passthrough hasn't necessarily ended. Merge with
               * partial chunk, leave state unchanged.
               */
               add_to_partial (context, context->start, context->iter);
            }
          else
            {
              /* The passthrough has ended at the close angle. Combine
               * it with the partial chunk if any. Call the passthrough
               * callback. Note that the open/close angles are
               * included in the text of the passthrough.
               */
              GError *tmp_error = NULL;

              advance_char (context); /* advance past close angle */
              add_to_partial (context, context->start, context->iter);

              if (context->flags & G_MARKUP_TREAT_CDATA_AS_TEXT &&
                  strncmp (context->partial_chunk->str, "<![CDATA[", 9) == 0)
                {
                  if (context->parser->text &&
                      text_validate (context,
                                     context->partial_chunk->str + 9,
                                     context->partial_chunk->len - 12,
                                     error))
                    (*context->parser->text) (context,
                                              context->partial_chunk->str + 9,
                                              context->partial_chunk->len - 12,
                                              context->user_data,
                                              &tmp_error);
                }
              else if (context->parser->passthrough &&
                       text_validate (context,
                                      context->partial_chunk->str,
                                      context->partial_chunk->len,
                                      error))
                (*context->parser->passthrough) (context,
                                                 context->partial_chunk->str,
                                                 context->partial_chunk->len,
                                                 context->user_data,
                                                 &tmp_error);

              truncate_partial (context);

              if (tmp_error == NULL)
                {
                  context->state = STATE_AFTER_CLOSE_ANGLE;
                  context->start = context->iter; /* could begin text */
                }
              else
                propagate_error (context, error, tmp_error);
            }
          break;

        case STATE_ERROR:
          goto finished;
          break;

        default:
          g_assert_not_reached ();
          break;
        }
    }

 finished:
  context->parsing = FALSE;

  return context->state != STATE_ERROR;
}