flatpak_context_reset_non_permissions (FlatpakContext *context)
{
  g_hash_table_remove_all (context->env_vars);
}