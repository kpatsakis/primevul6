slow_name_validate (GMarkupParseContext  *context,
                    const gchar          *name,
                    GError              **error)
{
  const gchar *p = name;

  if (!g_utf8_validate (name, strlen (name), NULL))
    {
      set_error (context, error, G_MARKUP_ERROR_BAD_UTF8,
                 _("Invalid UTF-8 encoded text in name — not valid “%s”"), name);
      return FALSE;
    }

  if (!(g_ascii_isalpha (*p) ||
        (!IS_COMMON_NAME_END_CHAR (*p) &&
         (*p == '_' ||
          *p == ':' ||
          g_unichar_isalpha (g_utf8_get_char (p))))))
    {
      set_error (context, error, G_MARKUP_ERROR_PARSE,
                 _("“%s” is not a valid name"), name);
      return FALSE;
    }

  for (p = g_utf8_next_char (name); *p != '\0'; p = g_utf8_next_char (p))
    {
      /* is_name_char */
      if (!(g_ascii_isalnum (*p) ||
            (!IS_COMMON_NAME_END_CHAR (*p) &&
             (*p == '.' ||
              *p == '-' ||
              *p == '_' ||
              *p == ':' ||
              g_unichar_isalpha (g_utf8_get_char (p))))))
        {
          set_error (context, error, G_MARKUP_ERROR_PARSE,
                     _("“%s” is not a valid name: “%c”"), name, *p);
          return FALSE;
        }
    }
  return TRUE;
}