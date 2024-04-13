exo_open_find_scheme (const gchar *string)
{
  gchar *current_dir;
  gchar *uri;
  gchar *path;

  /* is an absolute path, return file uri */
  if (g_path_is_absolute (string))
    return exo_open_get_path (string);

  /* treat it like a relative path */
  current_dir = g_get_current_dir ();
  path = g_build_filename (current_dir, string, NULL);
  g_free (current_dir);

  /* verify that a file of the given name exists */
  if (g_file_test (path, G_FILE_TEST_EXISTS))
    {
       uri = exo_open_get_path (path);
       g_free (path);
       return uri;
    }
  g_free (path);

  /* regular expression to check if it looks like an email address */
  if (g_regex_match_simple (MATCH_PATTERN_EMAIL, string, G_REGEX_CASELESS, 0))
    return g_strconcat ("mailto:", string, NULL);

  /* regular expression to check if it looks like an url, we don't need to check
   * for a complete url (http://) because this is already matched by the
   * exo_str_looks_like_an_uri() test */
  if (g_regex_match_simple (MATCH_PATTERN_HTTP, string, G_REGEX_CASELESS, 0))
    return g_strconcat ("http://", string, NULL);

  return NULL;
}