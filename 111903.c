flatpak_context_get_needs_session_bus_proxy (FlatpakContext *context)
{
  return g_hash_table_size (context->session_bus_policy) > 0;
}