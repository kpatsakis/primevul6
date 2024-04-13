encode_locals (MonoDynamicImage *assembly, MonoReflectionILGen *ilgen)
{
	MonoDynamicTable *table;
	guint32 *values;
	guint32 idx, sig_idx;
	guint nl = mono_array_length (ilgen->locals);
	SigBuffer buf;
	int i;

	sigbuffer_init (&buf, 32);
	sigbuffer_add_value (&buf, 0x07);
	sigbuffer_add_value (&buf, nl);
	for (i = 0; i < nl; ++i) {
		MonoReflectionLocalBuilder *lb = mono_array_get (ilgen->locals, MonoReflectionLocalBuilder*, i);
		
		if (lb->is_pinned)
			sigbuffer_add_value (&buf, MONO_TYPE_PINNED);
		
		encode_reflection_type (assembly, (MonoReflectionType*)lb->type, &buf);
	}
	sig_idx = sigbuffer_add_to_blob_cached (assembly, &buf);
	sigbuffer_free (&buf);

	if (assembly->standalonesig_cache == NULL)
		assembly->standalonesig_cache = g_hash_table_new (NULL, NULL);
	idx = GPOINTER_TO_UINT (g_hash_table_lookup (assembly->standalonesig_cache, GUINT_TO_POINTER (sig_idx)));
	if (idx)
		return idx;

	table = &assembly->tables [MONO_TABLE_STANDALONESIG];
	idx = table->next_idx ++;
	table->rows ++;
	alloc_table (table, table->rows);
	values = table->values + idx * MONO_STAND_ALONE_SIGNATURE_SIZE;

	values [MONO_STAND_ALONE_SIGNATURE] = sig_idx;

	g_hash_table_insert (assembly->standalonesig_cache, GUINT_TO_POINTER (sig_idx), GUINT_TO_POINTER (idx));

	return idx;
}