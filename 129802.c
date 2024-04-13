mono_image_add_memberef_row (MonoDynamicImage *assembly, guint32 parent, const char *name, guint32 sig)
{
	MonoDynamicTable *table;
	guint32 *values;
	guint32 token, pclass;

	switch (parent & MONO_TYPEDEFORREF_MASK) {
	case MONO_TYPEDEFORREF_TYPEREF:
		pclass = MONO_MEMBERREF_PARENT_TYPEREF;
		break;
	case MONO_TYPEDEFORREF_TYPESPEC:
		pclass = MONO_MEMBERREF_PARENT_TYPESPEC;
		break;
	case MONO_TYPEDEFORREF_TYPEDEF:
		pclass = MONO_MEMBERREF_PARENT_TYPEDEF;
		break;
	default:
		g_warning ("unknown typeref or def token 0x%08x for %s", parent, name);
		return 0;
	}
	/* extract the index */
	parent >>= MONO_TYPEDEFORREF_BITS;

	table = &assembly->tables [MONO_TABLE_MEMBERREF];

	if (assembly->save) {
		alloc_table (table, table->rows + 1);
		values = table->values + table->next_idx * MONO_MEMBERREF_SIZE;
		values [MONO_MEMBERREF_CLASS] = pclass | (parent << MONO_MEMBERREF_PARENT_BITS);
		values [MONO_MEMBERREF_NAME] = string_heap_insert (&assembly->sheap, name);
		values [MONO_MEMBERREF_SIGNATURE] = sig;
	}

	token = MONO_TOKEN_MEMBER_REF | table->next_idx;
	table->next_idx ++;

	return token;
}