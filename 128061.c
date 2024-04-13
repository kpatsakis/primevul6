text_validate (GMarkupParseContext  *context,
               const gchar          *p,
               gint                  len,
               GError              **error)
{
  if (!g_utf8_validate (p, len, NULL))
    {
      set_error (context, error, G_MARKUP_ERROR_BAD_UTF8,
                 _("Invalid UTF-8 encoded text in name — not valid “%s”"), p);
      return FALSE;
    }
  else
    return TRUE;
}