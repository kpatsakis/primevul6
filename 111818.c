adds_generic_policy (GHashTable *old, GHashTable *new)
{
  GLNX_HASH_TABLE_FOREACH_KV (new, const char *, key, GPtrArray *, new_values)
    {
      GPtrArray *old_values = g_hash_table_lookup (old, key);
      int i;

      if (new_values == NULL || new_values->len == 0)
        continue;

      if (old_values == NULL || old_values->len == 0)
        return TRUE;

      for (i = 0; i < new_values->len; i++)
        {
          const char *new_value = g_ptr_array_index (new_values, i);

          if (!flatpak_g_ptr_array_contains_string (old_values, new_value))
            return TRUE;
        }
    }

  return FALSE;
}