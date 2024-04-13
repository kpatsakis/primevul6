resolution_scope_from_image (MonoDynamicImage *assembly, MonoImage *image)
{
	MonoDynamicTable *table;
	guint32 token;
	guint32 *values;
	guint32 cols [MONO_ASSEMBLY_SIZE];
	const char *pubkey;
	guint32 publen;

	if ((token = GPOINTER_TO_UINT (g_hash_table_lookup (assembly->handleref, image))))
		return token;

	if (image->assembly->dynamic && (image->assembly == assembly->image.assembly)) {
		table = &assembly->tables [MONO_TABLE_MODULEREF];
		token = table->next_idx ++;
		table->rows ++;
		alloc_table (table, table->rows);
		values = table->values + token * MONO_MODULEREF_SIZE;
		values [MONO_MODULEREF_NAME] = string_heap_insert (&assembly->sheap, image->module_name);

		token <<= MONO_RESOLTION_SCOPE_BITS;
		token |= MONO_RESOLTION_SCOPE_MODULEREF;
		g_hash_table_insert (assembly->handleref, image, GUINT_TO_POINTER (token));

		return token;
	}
	
	if (image->assembly->dynamic)
		/* FIXME: */
		memset (cols, 0, sizeof (cols));
	else {
		/* image->assembly->image is the manifest module */
		image = image->assembly->image;
		mono_metadata_decode_row (&image->tables [MONO_TABLE_ASSEMBLY], 0, cols, MONO_ASSEMBLY_SIZE);
	}

	table = &assembly->tables [MONO_TABLE_ASSEMBLYREF];
	token = table->next_idx ++;
	table->rows ++;
	alloc_table (table, table->rows);
	values = table->values + token * MONO_ASSEMBLYREF_SIZE;
	values [MONO_ASSEMBLYREF_NAME] = string_heap_insert (&assembly->sheap, image->assembly_name);
	values [MONO_ASSEMBLYREF_MAJOR_VERSION] = cols [MONO_ASSEMBLY_MAJOR_VERSION];
	values [MONO_ASSEMBLYREF_MINOR_VERSION] = cols [MONO_ASSEMBLY_MINOR_VERSION];
	values [MONO_ASSEMBLYREF_BUILD_NUMBER] = cols [MONO_ASSEMBLY_BUILD_NUMBER];
	values [MONO_ASSEMBLYREF_REV_NUMBER] = cols [MONO_ASSEMBLY_REV_NUMBER];
	values [MONO_ASSEMBLYREF_FLAGS] = 0;
	values [MONO_ASSEMBLYREF_CULTURE] = 0;
	values [MONO_ASSEMBLYREF_HASH_VALUE] = 0;

	if (strcmp ("", image->assembly->aname.culture)) {
		values [MONO_ASSEMBLYREF_CULTURE] = string_heap_insert (&assembly->sheap,
				image->assembly->aname.culture);
	}

	if ((pubkey = mono_image_get_public_key (image, &publen))) {
		guchar pubtoken [9];
		pubtoken [0] = 8;
		mono_digest_get_public_token (pubtoken + 1, (guchar*)pubkey, publen);
		values [MONO_ASSEMBLYREF_PUBLIC_KEY] = mono_image_add_stream_data (&assembly->blob, (char*)pubtoken, 9);
	} else {
		values [MONO_ASSEMBLYREF_PUBLIC_KEY] = 0;
	}
	token <<= MONO_RESOLTION_SCOPE_BITS;
	token |= MONO_RESOLTION_SCOPE_ASSEMBLYREF;
	g_hash_table_insert (assembly->handleref, image, GUINT_TO_POINTER (token));
	return token;
}