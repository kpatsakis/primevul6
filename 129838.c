mono_image_add_cattrs (MonoDynamicImage *assembly, guint32 idx, guint32 type, MonoArray *cattrs)
{
	MonoDynamicTable *table;
	MonoReflectionCustomAttr *cattr;
	guint32 *values;
	guint32 count, i, token;
	char blob_size [6];
	char *p = blob_size;
	
	/* it is legal to pass a NULL cattrs: we avoid to use the if in a lot of places */
	if (!cattrs)
		return;
	count = mono_array_length (cattrs);
	table = &assembly->tables [MONO_TABLE_CUSTOMATTRIBUTE];
	table->rows += count;
	alloc_table (table, table->rows);
	values = table->values + table->next_idx * MONO_CUSTOM_ATTR_SIZE;
	idx <<= MONO_CUSTOM_ATTR_BITS;
	idx |= type;
	for (i = 0; i < count; ++i) {
		cattr = (MonoReflectionCustomAttr*)mono_array_get (cattrs, gpointer, i);
		values [MONO_CUSTOM_ATTR_PARENT] = idx;
		token = mono_image_create_token (assembly, (MonoObject*)cattr->ctor, FALSE, FALSE);
		type = mono_metadata_token_index (token);
		type <<= MONO_CUSTOM_ATTR_TYPE_BITS;
		switch (mono_metadata_token_table (token)) {
		case MONO_TABLE_METHOD:
			type |= MONO_CUSTOM_ATTR_TYPE_METHODDEF;
			break;
		case MONO_TABLE_MEMBERREF:
			type |= MONO_CUSTOM_ATTR_TYPE_MEMBERREF;
			break;
		default:
			g_warning ("got wrong token in custom attr");
			continue;
		}
		values [MONO_CUSTOM_ATTR_TYPE] = type;
		p = blob_size;
		mono_metadata_encode_value (mono_array_length (cattr->data), p, &p);
		values [MONO_CUSTOM_ATTR_VALUE] = add_to_blob_cached (assembly, blob_size, p - blob_size,
			mono_array_addr (cattr->data, char, 0), mono_array_length (cattr->data));
		values += MONO_CUSTOM_ATTR_SIZE;
		++table->next_idx;
	}
}