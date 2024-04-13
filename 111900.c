adds_bus_policy (GHashTable *old, GHashTable *new)
{
  GLNX_HASH_TABLE_FOREACH_KV (new, const char *, name, gpointer, _new_policy)
    {
      int new_policy = GPOINTER_TO_INT (_new_policy);
      int old_policy = GPOINTER_TO_INT (g_hash_table_lookup (old, name));
      if (new_policy > old_policy)
        return TRUE;
    }

  return FALSE;
}