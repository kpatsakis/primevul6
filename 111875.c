flatpak_context_remove_sockets (FlatpakContext       *context,
                                FlatpakContextSockets sockets)
{
  context->sockets_valid |= sockets;
  context->sockets &= ~sockets;
}