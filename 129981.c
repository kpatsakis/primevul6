assembly_add_resource (MonoReflectionModuleBuilder *mb, MonoDynamicImage *assembly, MonoReflectionResource *rsrc)
{
	MonoDynamicTable *table;
	guint32 *values;
	char blob_size [6];
	guchar hash [20];
	char *b = blob_size;
	char *name, *sname;
	guint32 idx, offset;

	if (rsrc->filename) {
		name = mono_string_to_utf8 (rsrc->filename);
		sname = g_path_get_basename (name);
	
		table = &assembly->tables [MONO_TABLE_FILE];
		table->rows++;
		alloc_table (table, table->rows);
		values = table->values + table->next_idx * MONO_FILE_SIZE;
		values [MONO_FILE_FLAGS] = FILE_CONTAINS_NO_METADATA;
		values [MONO_FILE_NAME] = string_heap_insert (&assembly->sheap, sname);
		g_free (sname);

		mono_sha1_get_digest_from_file (name, hash);
		mono_metadata_encode_value (20, b, &b);
		values [MONO_FILE_HASH_VALUE] = mono_image_add_stream_data (&assembly->blob, blob_size, b-blob_size);
		mono_image_add_stream_data (&assembly->blob, (char*)hash, 20);
		g_free (name);
		idx = table->next_idx++;
		rsrc->offset = 0;
		idx = MONO_IMPLEMENTATION_FILE | (idx << MONO_IMPLEMENTATION_BITS);
	} else {
		char sizebuf [4];
		char *data;
		guint len;
		if (rsrc->data) {
			data = mono_array_addr (rsrc->data, char, 0);
			len = mono_array_length (rsrc->data);
		} else {
			data = NULL;
			len = 0;
		}
		offset = len;
		sizebuf [0] = offset; sizebuf [1] = offset >> 8;
		sizebuf [2] = offset >> 16; sizebuf [3] = offset >> 24;
		rsrc->offset = mono_image_add_stream_data (&assembly->resources, sizebuf, 4);
		mono_image_add_stream_data (&assembly->resources, data, len);

		if (!mb->is_main)
			/* 
			 * The entry should be emitted into the MANIFESTRESOURCE table of 
			 * the main module, but that needs to reference the FILE table
			 * which isn't emitted yet.
			 */
			return;
		else
			idx = 0;
	}

	assembly_add_resource_manifest (mb, assembly, rsrc, idx);
}