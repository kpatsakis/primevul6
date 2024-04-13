ephy_string_commandline_args_to_uris (char   **arguments,
                                      GError **error)
{
  gchar **args;
  GFile *file;
  guint i;

  if (arguments == NULL)
    return NULL;

  args = g_malloc0 (sizeof (gchar *) * (g_strv_length (arguments) + 1));

  for (i = 0; arguments[i] != NULL; ++i) {
    file = g_file_new_for_commandline_arg (arguments [i]);
    if (g_file_is_native (file) && g_file_query_exists (file, NULL)) {
      args[i] = g_file_get_uri (file);
    } else {
      args[i] = g_locale_to_utf8 (arguments [i], -1,
                                  NULL, NULL, error);
      if (error && *error) {
        g_strfreev (args);
        return NULL;
      }
    }
    g_object_unref (file);
  }

  return args;
}