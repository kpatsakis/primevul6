option_env_fd_cb (const gchar *option_name,
                  const gchar *value,
                  gpointer     data,
                  GError     **error)
{
  FlatpakContext *context = data;
  guint64 fd;
  gchar *endptr;
  gboolean ret;

  fd = g_ascii_strtoull (value, &endptr, 10);

  if (endptr == NULL || *endptr != '\0' || fd > G_MAXINT)
    return glnx_throw (error, "Not a valid file descriptor: %s", value);

  ret = flatpak_context_parse_env_fd (context, (int) fd, error);

  if (fd >= 3)
    close (fd);

  return ret;
}