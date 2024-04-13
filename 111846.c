option_socket_cb (const gchar *option_name,
                  const gchar *value,
                  gpointer     data,
                  GError     **error)
{
  FlatpakContext *context = data;
  FlatpakContextSockets socket;

  socket = flatpak_context_socket_from_string (value, error);
  if (socket == 0)
    return FALSE;

  if (socket == FLATPAK_CONTEXT_SOCKET_FALLBACK_X11)
    socket |= FLATPAK_CONTEXT_SOCKET_X11;

  flatpak_context_add_sockets (context, socket);

  return TRUE;
}