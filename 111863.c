flatpak_context_make_sandboxed (FlatpakContext *context)
{
  /* We drop almost everything from the app permission, except
   * multiarch which is inherited, to make sure app code keeps
   * running. */
  context->shares_valid &= 0;
  context->sockets_valid &= 0;
  context->devices_valid &= 0;
  context->features_valid &= FLATPAK_CONTEXT_FEATURE_MULTIARCH;

  context->shares &= context->shares_valid;
  context->sockets &= context->sockets_valid;
  context->devices &= context->devices_valid;
  context->features &= context->features_valid;

  g_hash_table_remove_all (context->persistent);
  g_hash_table_remove_all (context->filesystems);
  g_hash_table_remove_all (context->session_bus_policy);
  g_hash_table_remove_all (context->system_bus_policy);
  g_hash_table_remove_all (context->generic_policy);
}