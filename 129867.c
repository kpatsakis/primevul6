add_to_blob_cached (MonoDynamicImage *assembly, char *b1, int s1, char *b2, int s2)
{
	guint32 idx;
	char *copy;
	gpointer oldkey, oldval;

	copy = g_malloc (s1+s2);
	memcpy (copy, b1, s1);
	memcpy (copy + s1, b2, s2);
	if (g_hash_table_lookup_extended (assembly->blob_cache, copy, &oldkey, &oldval)) {
		g_free (copy);
		idx = GPOINTER_TO_UINT (oldval);
	} else {
		idx = mono_image_add_stream_data (&assembly->blob, b1, s1);
		mono_image_add_stream_data (&assembly->blob, b2, s2);
		g_hash_table_insert (assembly->blob_cache, copy, GUINT_TO_POINTER (idx));
	}
	return idx;
}