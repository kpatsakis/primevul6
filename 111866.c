flatpak_context_reset_permissions (FlatpakContext *context)
{
  context->shares_valid = 0;
  context->sockets_valid = 0;
  context->devices_valid = 0;
  context->features_valid = 0;

  context->shares = 0;
  context->sockets = 0;
  context->devices = 0;
  context->features = 0;

  g_hash_table_remove_all (context->persistent);
  g_hash_table_remove_all (context->filesystems);
  g_hash_table_remove_all (context->session_bus_policy);
  g_hash_table_remove_all (context->system_bus_policy);
  g_hash_table_remove_all (context->generic_policy);
}