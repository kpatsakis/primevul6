mono_image_get_sighelper_token (MonoDynamicImage *assembly, MonoReflectionSigHelper *helper)
{
	guint32 idx;
	MonoDynamicTable *table;
	guint32 *values;

	table = &assembly->tables [MONO_TABLE_STANDALONESIG];
	idx = table->next_idx ++;
	table->rows ++;
	alloc_table (table, table->rows);
	values = table->values + idx * MONO_STAND_ALONE_SIGNATURE_SIZE;

	values [MONO_STAND_ALONE_SIGNATURE] =
		mono_reflection_encode_sighelper (assembly, helper);

	return idx;
}