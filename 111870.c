flatpak_context_add_sockets (FlatpakContext       *context,
                             FlatpakContextSockets sockets)
{
  context->sockets_valid |= sockets;
  context->sockets |= sockets;
}