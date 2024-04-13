ephy_string_get_host_name (const char *url)
{
  g_autoptr (GUri) uri = NULL;

  if (url == NULL ||
      g_str_has_prefix (url, "file://") ||
      g_str_has_prefix (url, "about:") ||
      g_str_has_prefix (url, "ephy-about:"))
    return NULL;

  uri = g_uri_parse (url, G_URI_FLAGS_NONE, NULL);
  /* If uri is NULL it's very possible that we just got
   * something without a scheme, let's try to prepend
   * 'http://' */
  if (uri == NULL) {
    char *effective_url = g_strconcat ("http://", url, NULL);
    uri = g_uri_parse (effective_url, G_URI_FLAGS_NONE, NULL);
    g_free (effective_url);
  }

  if (uri == NULL)
    return NULL;

  return g_strdup (g_uri_get_host (uri));
}