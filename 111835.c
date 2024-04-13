flatpak_context_parse_env_fd (FlatpakContext *context,
                              int fd,
                              GError **error)
{
  g_autoptr(GBytes) env_block = NULL;
  const char *data;
  gsize len;

  env_block = glnx_fd_readall_bytes (fd, NULL, error);

  if (env_block == NULL)
    return FALSE;

  data = g_bytes_get_data (env_block, &len);
  return flatpak_context_parse_env_block (context, data, len, error);
}