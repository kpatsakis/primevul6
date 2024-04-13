flatpak_context_sockets_to_string (FlatpakContextSockets sockets, FlatpakContextSockets valid)
{
  return flatpak_context_bitmask_to_string (sockets, valid, flatpak_context_sockets);
}