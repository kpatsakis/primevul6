flatpak_context_free (FlatpakContext *context)
{
  g_hash_table_destroy (context->env_vars);
  g_hash_table_destroy (context->persistent);
  g_hash_table_destroy (context->filesystems);
  g_hash_table_destroy (context->session_bus_policy);
  g_hash_table_destroy (context->system_bus_policy);
  g_hash_table_destroy (context->generic_policy);
  g_slice_free (FlatpakContext, context);
}