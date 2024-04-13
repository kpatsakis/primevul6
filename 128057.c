set_unescape_error (GMarkupParseContext  *context,
                    GError              **error,
                    const gchar          *remaining_text,
                    GMarkupError          code,
                    const gchar          *format,
                    ...)
{
  GError *tmp_error;
  gchar *s;
  va_list args;
  gint remaining_newlines;
  const gchar *p;

  remaining_newlines = 0;
  p = remaining_text;
  while (*p != '\0')
    {
      if (*p == '\n')
        ++remaining_newlines;
      ++p;
    }

  va_start (args, format);
  s = g_strdup_vprintf (format, args);
  va_end (args);

  tmp_error = g_error_new (G_MARKUP_ERROR,
                           code,
                           _("Error on line %d: %s"),
                           context->line_number - remaining_newlines,
                           s);

  g_free (s);

  mark_error (context, tmp_error);

  g_propagate_error (error, tmp_error);
}