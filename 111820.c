flatpak_context_get_exports (FlatpakContext *context,
                             const char     *app_id)
{
  g_autoptr(GFile) app_id_dir = flatpak_get_data_dir (app_id);

  return flatpak_context_get_exports_full (context,
                                           app_id_dir, NULL,
                                           FALSE, FALSE, NULL, NULL);
}