mono_module_file_get_object (MonoDomain *domain, MonoImage *image, int table_index)
{
	static MonoClass *module_type;
	MonoReflectionModule *res;
	MonoTableInfo *table;
	guint32 cols [MONO_FILE_SIZE];
	const char *name;
	guint32 i, name_idx;
	const char *val;
	
	if (!module_type) {
		MonoClass *class = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "MonoModule");
		if (class == NULL)
			class = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "Module");
		g_assert (class);
		module_type = class;
	}
	res = (MonoReflectionModule *)mono_object_new (domain, module_type);

	table = &image->tables [MONO_TABLE_FILE];
	g_assert (table_index < table->rows);
	mono_metadata_decode_row (table, table_index, cols, MONO_FILE_SIZE);

	res->image = NULL;
	MONO_OBJECT_SETREF (res, assembly, (MonoReflectionAssembly *) mono_assembly_get_object(domain, image->assembly));
	name = mono_metadata_string_heap (image, cols [MONO_FILE_NAME]);

	/* Check whenever the row has a corresponding row in the moduleref table */
	table = &image->tables [MONO_TABLE_MODULEREF];
	for (i = 0; i < table->rows; ++i) {
		name_idx = mono_metadata_decode_row_col (table, i, MONO_MODULEREF_NAME);
		val = mono_metadata_string_heap (image, name_idx);
		if (strcmp (val, name) == 0)
			res->image = image->modules [i];
	}

	MONO_OBJECT_SETREF (res, fqname, mono_string_new (domain, name));
	MONO_OBJECT_SETREF (res, name, mono_string_new (domain, name));
	MONO_OBJECT_SETREF (res, scopename, mono_string_new (domain, name));
	res->is_resource = cols [MONO_FILE_FLAGS] && FILE_CONTAINS_NO_METADATA;
	res->token = mono_metadata_make_token (MONO_TABLE_FILE, table_index + 1);

	return res;
}