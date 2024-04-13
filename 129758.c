mono_image_add_decl_security (MonoDynamicImage *assembly, guint32 parent_token, MonoArray *permissions)
{
	MonoDynamicTable *table;
	guint32 *values;
	guint32 count, i, idx;
	MonoReflectionPermissionSet *perm;

	if (!permissions)
		return;

	count = mono_array_length (permissions);
	table = &assembly->tables [MONO_TABLE_DECLSECURITY];
	table->rows += count;
	alloc_table (table, table->rows);

	for (i = 0; i < mono_array_length (permissions); ++i) {
		perm = (MonoReflectionPermissionSet*)mono_array_addr (permissions, MonoReflectionPermissionSet, i);

		values = table->values + table->next_idx * MONO_DECL_SECURITY_SIZE;

		idx = mono_metadata_token_index (parent_token);
		idx <<= MONO_HAS_DECL_SECURITY_BITS;
		switch (mono_metadata_token_table (parent_token)) {
		case MONO_TABLE_TYPEDEF:
			idx |= MONO_HAS_DECL_SECURITY_TYPEDEF;
			break;
		case MONO_TABLE_METHOD:
			idx |= MONO_HAS_DECL_SECURITY_METHODDEF;
			break;
		case MONO_TABLE_ASSEMBLY:
			idx |= MONO_HAS_DECL_SECURITY_ASSEMBLY;
			break;
		default:
			g_assert_not_reached ();
		}

		values [MONO_DECL_SECURITY_ACTION] = perm->action;
		values [MONO_DECL_SECURITY_PARENT] = idx;
		values [MONO_DECL_SECURITY_PERMISSIONSET] = add_mono_string_to_blob_cached (assembly, perm->pset);

		++table->next_idx;
	}
}