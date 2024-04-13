flatpak_context_get_session_bus_policy_allowed_own_names (FlatpakContext *context)
{
  GHashTableIter iter;
  gpointer key, value;
  g_autoptr(GPtrArray) names = g_ptr_array_new_with_free_func (g_free);

  g_hash_table_iter_init (&iter, context->session_bus_policy);
  while (g_hash_table_iter_next (&iter, &key, &value))
    if (GPOINTER_TO_INT (value) == FLATPAK_POLICY_OWN)
      g_ptr_array_add (names, g_strdup (key));

  g_ptr_array_add (names, NULL);
  return (GStrv) g_ptr_array_free (g_steal_pointer (&names), FALSE);
}