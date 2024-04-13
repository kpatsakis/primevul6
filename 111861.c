flatpak_context_set_system_bus_policy (FlatpakContext *context,
                                       const char     *name,
                                       FlatpakPolicy   policy)
{
  g_hash_table_insert (context->system_bus_policy, g_strdup (name), GINT_TO_POINTER (policy));
}