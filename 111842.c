flatpak_context_allow_host_fs (FlatpakContext *context)
{
  flatpak_context_take_filesystem (context, g_strdup ("host"), FLATPAK_FILESYSTEM_MODE_READ_WRITE);
}