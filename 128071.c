emit_start_element (GMarkupParseContext  *context,
                    GError              **error)
{
  int i, j = 0;
  const gchar *start_name;
  const gchar **attr_names;
  const gchar **attr_values;
  GError *tmp_error;

  /* In case we want to ignore qualified tags and we see that we have
   * one here, we push a subparser.  This will ignore all tags inside of
   * the qualified tag.
   *
   * We deal with the end of the subparser from emit_end_element.
   */
  if ((context->flags & G_MARKUP_IGNORE_QUALIFIED) && strchr (current_element (context), ':'))
    {
      static const GMarkupParser ignore_parser;
      g_markup_parse_context_push (context, &ignore_parser, NULL);
      clear_attributes (context);
      return;
    }

  attr_names = g_newa (const gchar *, context->cur_attr + 2);
  attr_values = g_newa (const gchar *, context->cur_attr + 2);
  for (i = 0; i < context->cur_attr + 1; i++)
    {
      /* Possibly omit qualified attribute names from the list */
      if ((context->flags & G_MARKUP_IGNORE_QUALIFIED) && strchr (context->attr_names[i]->str, ':'))
        continue;

      attr_names[j] = context->attr_names[i]->str;
      attr_values[j] = context->attr_values[i]->str;
      j++;
    }
  attr_names[j] = NULL;
  attr_values[j] = NULL;

  /* Call user callback for element start */
  tmp_error = NULL;
  start_name = current_element (context);

  if (context->parser->start_element &&
      name_validate (context, start_name, error))
    (* context->parser->start_element) (context,
                                        start_name,
                                        (const gchar **)attr_names,
                                        (const gchar **)attr_values,
                                        context->user_data,
                                        &tmp_error);
  clear_attributes (context);

  if (tmp_error != NULL)
    propagate_error (context, error, tmp_error);
}