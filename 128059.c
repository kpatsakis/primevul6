utf8_str (const gchar *utf8,
          gchar       *buf)
{
  gunichar c = g_utf8_get_char_validated (utf8, -1);
  if (c == (gunichar) -1 || c == (gunichar) -2)
    {
      gchar *temp = g_strdup_printf ("\\x%02x", (guint)(guchar)*utf8);
      memset (buf, 0, 8);
      memcpy (buf, temp, strlen (temp));
      g_free (temp);
    }
  else
    char_str (c, buf);
  return buf;
}