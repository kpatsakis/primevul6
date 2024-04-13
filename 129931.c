fixup_cattrs (MonoDynamicImage *assembly)
{
	MonoDynamicTable *table;
	guint32 *values;
	guint32 type, i, idx, token;
	MonoObject *ctor;

	table = &assembly->tables [MONO_TABLE_CUSTOMATTRIBUTE];

	for (i = 0; i < table->rows; ++i) {
		values = table->values + ((i + 1) * MONO_CUSTOM_ATTR_SIZE);

		type = values [MONO_CUSTOM_ATTR_TYPE];
		if ((type & MONO_CUSTOM_ATTR_TYPE_MASK) == MONO_CUSTOM_ATTR_TYPE_METHODDEF) {
			idx = type >> MONO_CUSTOM_ATTR_TYPE_BITS;
			token = mono_metadata_make_token (MONO_TABLE_METHOD, idx);
			ctor = mono_g_hash_table_lookup (assembly->tokens, GUINT_TO_POINTER (token));
			g_assert (ctor);

			if (!strcmp (ctor->vtable->klass->name, "MonoCMethod")) {
				MonoMethod *m = ((MonoReflectionMethod*)ctor)->method;
				idx = GPOINTER_TO_UINT (g_hash_table_lookup (assembly->method_to_table_idx, m));
				values [MONO_CUSTOM_ATTR_TYPE] = (idx << MONO_CUSTOM_ATTR_TYPE_BITS) | MONO_CUSTOM_ATTR_TYPE_METHODDEF;
			}
		}
	}
}