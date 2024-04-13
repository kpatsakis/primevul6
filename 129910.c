mono_image_get_varargs_method_token (MonoDynamicImage *assembly, guint32 original,
				     const gchar *name, guint32 sig)
{
	MonoDynamicTable *table;
	guint32 token;
	guint32 *values;
	
	table = &assembly->tables [MONO_TABLE_MEMBERREF];

	if (assembly->save) {
		alloc_table (table, table->rows + 1);
		values = table->values + table->next_idx * MONO_MEMBERREF_SIZE;
		values [MONO_MEMBERREF_CLASS] = original;
		values [MONO_MEMBERREF_NAME] = string_heap_insert (&assembly->sheap, name);
		values [MONO_MEMBERREF_SIGNATURE] = sig;
	}

	token = MONO_TOKEN_MEMBER_REF | table->next_idx;
	table->next_idx ++;

	return token;
}