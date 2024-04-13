unescape_gstring_inplace (GMarkupParseContext  *context,
                          GString              *string,
                          gboolean             *is_ascii,
                          GError              **error)
{
  char mask, *to;
  const char *from;
  gboolean normalize_attribute;

  *is_ascii = FALSE;

  /* are we unescaping an attribute or not ? */
  if (context->state == STATE_INSIDE_ATTRIBUTE_VALUE_SQ ||
      context->state == STATE_INSIDE_ATTRIBUTE_VALUE_DQ)
    normalize_attribute = TRUE;
  else
    normalize_attribute = FALSE;

  /*
   * Meeks' theorem: unescaping can only shrink text.
   * for &lt; etc. this is obvious, for &#xffff; more
   * thought is required, but this is patently so.
   */
  mask = 0;
  for (from = to = string->str; *from != '\0'; from++, to++)
    {
      *to = *from;

      mask |= *to;
      if (normalize_attribute && (*to == '\t' || *to == '\n'))
        *to = ' ';
      if (*to == '\r')
        {
          *to = normalize_attribute ? ' ' : '\n';
          if (from[1] == '\n')
            from++;
        }
      if (*from == '&')
        {
          from++;
          if (*from == '#')
            {
              gint base = 10;
              gulong l;
              gchar *end = NULL;

              from++;

              if (*from == 'x')
                {
                  base = 16;
                  from++;
                }

              errno = 0;
              l = strtoul (from, &end, base);

              if (end == from || errno != 0)
                {
                  set_unescape_error (context, error,
                                      from, G_MARKUP_ERROR_PARSE,
                                      _("Failed to parse “%-.*s”, which "
                                        "should have been a digit "
                                        "inside a character reference "
                                        "(&#234; for example) — perhaps "
                                        "the digit is too large"),
                                      (int)(end - from), from);
                  return FALSE;
                }
              else if (*end != ';')
                {
                  set_unescape_error (context, error,
                                      from, G_MARKUP_ERROR_PARSE,
                                      _("Character reference did not end with a "
                                        "semicolon; "
                                        "most likely you used an ampersand "
                                        "character without intending to start "
                                        "an entity — escape ampersand as &amp;"));
                  return FALSE;
                }
              else
                {
                  /* characters XML 1.1 permits */
                  if ((0 < l && l <= 0xD7FF) ||
                      (0xE000 <= l && l <= 0xFFFD) ||
                      (0x10000 <= l && l <= 0x10FFFF))
                    {
                      gchar buf[8];
                      char_str (l, buf);
                      strcpy (to, buf);
                      to += strlen (buf) - 1;
                      from = end;
                      if (l >= 0x80) /* not ascii */
                        mask |= 0x80;
                    }
                  else
                    {
                      set_unescape_error (context, error,
                                          from, G_MARKUP_ERROR_PARSE,
                                          _("Character reference “%-.*s” does not "
                                            "encode a permitted character"),
                                          (int)(end - from), from);
                      return FALSE;
                    }
                }
            }

          else if (strncmp (from, "lt;", 3) == 0)
            {
              *to = '<';
              from += 2;
            }
          else if (strncmp (from, "gt;", 3) == 0)
            {
              *to = '>';
              from += 2;
            }
          else if (strncmp (from, "amp;", 4) == 0)
            {
              *to = '&';
              from += 3;
            }
          else if (strncmp (from, "quot;", 5) == 0)
            {
              *to = '"';
              from += 4;
            }
          else if (strncmp (from, "apos;", 5) == 0)
            {
              *to = '\'';
              from += 4;
            }
          else
            {
              if (*from == ';')
                set_unescape_error (context, error,
                                    from, G_MARKUP_ERROR_PARSE,
                                    _("Empty entity “&;” seen; valid "
                                      "entities are: &amp; &quot; &lt; &gt; &apos;"));
              else
                {
                  const char *end = strchr (from, ';');
                  if (end)
                    set_unescape_error (context, error,
                                        from, G_MARKUP_ERROR_PARSE,
                                        _("Entity name “%-.*s” is not known"),
                                        (int)(end - from), from);
                  else
                    set_unescape_error (context, error,
                                        from, G_MARKUP_ERROR_PARSE,
                                        _("Entity did not end with a semicolon; "
                                          "most likely you used an ampersand "
                                          "character without intending to start "
                                          "an entity — escape ampersand as &amp;"));
                }
              return FALSE;
            }
        }
    }

  g_assert (to - string->str <= string->len);
  if (to - string->str != string->len)
    g_string_truncate (string, to - string->str);

  *is_ascii = !(mask & 0x80);

  return TRUE;
}