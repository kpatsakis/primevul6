_flatpak_uri_equal (const char *uri1,
                    const char *uri2)
{
  g_autofree char *uri1_norm = NULL;
  g_autofree char *uri2_norm = NULL;
  gsize uri1_len = strlen (uri1);
  gsize uri2_len = strlen (uri2);

  /* URIs handled by libostree are equivalent with or without a trailing slash,
   * but this isn't otherwise guaranteed to be the case.
   */
  if (g_str_has_prefix (uri1, "oci+") || g_str_has_prefix (uri2, "oci+"))
    return g_strcmp0 (uri1, uri2) == 0;

  if (g_str_has_suffix (uri1, "/"))
    uri1_norm = g_strndup (uri1, uri1_len - 1);
  else
    uri1_norm = g_strdup (uri1);

  if (g_str_has_suffix (uri2, "/"))
    uri2_norm = g_strndup (uri2, uri2_len - 1);
  else
    uri2_norm = g_strdup (uri2);

  return g_strcmp0 (uri1_norm, uri2_norm) == 0;
}