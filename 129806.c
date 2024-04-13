mono_image_get_methodspec_token_for_generic_method_definition (MonoDynamicImage *assembly, MonoReflectionMethodBuilder *mb)
{
	MonoDynamicTable *table;
	guint32 *values;
	guint32 token, mtoken = 0;

	token = GPOINTER_TO_UINT (mono_g_hash_table_lookup (assembly->methodspec, mb));
	if (token)
		return token;

	table = &assembly->tables [MONO_TABLE_METHODSPEC];

	mtoken = mono_image_get_methodref_token_for_methodbuilder (assembly, mb);
	switch (mono_metadata_token_table (mtoken)) {
	case MONO_TABLE_MEMBERREF:
		mtoken = (mono_metadata_token_index (mtoken) << MONO_METHODDEFORREF_BITS) | MONO_METHODDEFORREF_METHODREF;
		break;
	case MONO_TABLE_METHOD:
		mtoken = (mono_metadata_token_index (mtoken) << MONO_METHODDEFORREF_BITS) | MONO_METHODDEFORREF_METHODDEF;
		break;
	default:
		g_assert_not_reached ();
	}

	if (assembly->save) {
		alloc_table (table, table->rows + 1);
		values = table->values + table->next_idx * MONO_METHODSPEC_SIZE;
		values [MONO_METHODSPEC_METHOD] = mtoken;
		values [MONO_METHODSPEC_SIGNATURE] = encode_generic_method_definition_sig (assembly, mb);
	}

	token = MONO_TOKEN_METHOD_SPEC | table->next_idx;
	table->next_idx ++;

	mono_g_hash_table_insert (assembly->methodspec, mb, GUINT_TO_POINTER(token));
	return token;
}