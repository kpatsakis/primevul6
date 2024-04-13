exo_open_get_path (const gchar *string)
{
  gchar *escaped;
  gchar *uri;
  escaped = g_uri_escape_string (string, G_URI_RESERVED_CHARS_ALLOWED_IN_PATH, TRUE);
  uri = g_strconcat ("file://", escaped, NULL);
  g_free (escaped);
  return uri;
}