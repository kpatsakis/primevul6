free_blob_cache_entry (gpointer key, gpointer val, gpointer user_data)
{
	g_free (key);
}