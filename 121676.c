remove_unless_decomposed_in_hash (gpointer key,
                                  gpointer value,
                                  gpointer user_data)
{
  GHashTable *table = user_data;
  const FlatpakDecomposed *d = key;

  return !g_hash_table_contains (table, d);
}