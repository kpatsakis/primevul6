flatpak_context_apply_generic_policy (FlatpakContext *context,
                                      const char     *key,
                                      const char     *value)
{
  GPtrArray *new = g_ptr_array_new ();
  const char **old_v;
  int i;

  g_assert (strchr (key, '.') != NULL);

  old_v = g_hash_table_lookup (context->generic_policy, key);
  for (i = 0; old_v != NULL && old_v[i] != NULL; i++)
    {
      const char *old = old_v[i];
      const char *cmp1 = old;
      const char *cmp2 = value;
      if (*cmp1 == '!')
        cmp1++;
      if (*cmp2 == '!')
        cmp2++;
      if (strcmp (cmp1, cmp2) != 0)
        g_ptr_array_add (new, g_strdup (old));
    }

  g_ptr_array_add (new, g_strdup (value));
  g_ptr_array_add (new, NULL);

  g_hash_table_insert (context->generic_policy, g_strdup (key),
                       g_ptr_array_free (new, FALSE));
}