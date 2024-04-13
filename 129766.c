string_heap_init (MonoDynamicStream *sh)
{
	sh->index = 0;
	sh->alloc_size = 4096;
	sh->data = g_malloc (4096);
	sh->hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
	string_heap_insert (sh, "");
}