flatpak_context_set_env_var (FlatpakContext *context,
                             const char     *name,
                             const char     *value)
{
  g_hash_table_insert (context->env_vars, g_strdup (name), g_strdup (value));
}