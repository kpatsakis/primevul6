option_filesystem_cb (const gchar *option_name,
                      const gchar *value,
                      gpointer     data,
                      GError     **error)
{
  FlatpakContext *context = data;
  g_autofree char *fs = NULL;
  FlatpakFilesystemMode mode;

  if (!flatpak_context_parse_filesystem (value, &fs, &mode, error))
    return FALSE;

  flatpak_context_take_filesystem (context, g_steal_pointer (&fs), mode);
  return TRUE;
}