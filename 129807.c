assembly_add_resource_manifest (MonoReflectionModuleBuilder *mb, MonoDynamicImage *assembly, MonoReflectionResource *rsrc, guint32 implementation)
{
	MonoDynamicTable *table;
	guint32 *values;

	table = &assembly->tables [MONO_TABLE_MANIFESTRESOURCE];
	table->rows++;
	alloc_table (table, table->rows);
	values = table->values + table->next_idx * MONO_MANIFEST_SIZE;
	values [MONO_MANIFEST_OFFSET] = rsrc->offset;
	values [MONO_MANIFEST_FLAGS] = rsrc->attrs;
	values [MONO_MANIFEST_NAME] = string_heap_insert_mstring (&assembly->sheap, rsrc->name);
	values [MONO_MANIFEST_IMPLEMENTATION] = implementation;
	table->next_idx++;
}