create_dynamic_mono_image (MonoDynamicAssembly *assembly, char *assembly_name, char *module_name)
{
	static const guchar entrycode [16] = {0xff, 0x25, 0};
	MonoDynamicImage *image;
	int i;

	const char *version;

	if (!strcmp (mono_get_runtime_info ()->framework_version, "2.1"))
		version = "v2.0.50727"; /* HACK: SL 2 enforces the .net 2 metadata version */
	else
		version = mono_get_runtime_info ()->runtime_version;

#if HAVE_BOEHM_GC
	/* The MonoGHashTable's need GC tracking */
	image = GC_MALLOC (sizeof (MonoDynamicImage));
#else
	image = g_new0 (MonoDynamicImage, 1);
#endif
	
	mono_profiler_module_event (&image->image, MONO_PROFILE_START_LOAD);
	
	/*g_print ("created image %p\n", image);*/
	/* keep in sync with image.c */
	image->image.name = assembly_name;
	image->image.assembly_name = image->image.name; /* they may be different */
	image->image.module_name = module_name;
	image->image.version = g_strdup (version);
	image->image.md_version_major = 1;
	image->image.md_version_minor = 1;
	image->image.dynamic = TRUE;

	image->image.references = g_new0 (MonoAssembly*, 1);
	image->image.references [0] = NULL;

	mono_image_init (&image->image);

	image->token_fixups = mono_g_hash_table_new_type ((GHashFunc)mono_object_hash, NULL, MONO_HASH_KEY_GC);
	image->method_to_table_idx = g_hash_table_new (NULL, NULL);
	image->field_to_table_idx = g_hash_table_new (NULL, NULL);
	image->method_aux_hash = g_hash_table_new (NULL, NULL);
	image->vararg_aux_hash = g_hash_table_new (NULL, NULL);
	image->handleref = g_hash_table_new (NULL, NULL);
	image->handleref_managed = mono_g_hash_table_new_type ((GHashFunc)mono_object_hash, NULL, MONO_HASH_KEY_GC);
	image->tokens = mono_g_hash_table_new_type (NULL, NULL, MONO_HASH_VALUE_GC);
	image->generic_def_objects = mono_g_hash_table_new_type (NULL, NULL, MONO_HASH_VALUE_GC);
	image->methodspec = mono_g_hash_table_new_type ((GHashFunc)mono_object_hash, NULL, MONO_HASH_KEY_GC);
	image->typespec = g_hash_table_new ((GHashFunc)mono_metadata_type_hash, (GCompareFunc)mono_metadata_type_equal);
	image->typeref = g_hash_table_new ((GHashFunc)mono_metadata_type_hash, (GCompareFunc)mono_metadata_type_equal);
	image->blob_cache = g_hash_table_new ((GHashFunc)mono_blob_entry_hash, (GCompareFunc)mono_blob_entry_equal);
	image->gen_params = g_ptr_array_new ();

	/*g_print ("string heap create for image %p (%s)\n", image, module_name);*/
	string_heap_init (&image->sheap);
	mono_image_add_stream_data (&image->us, "", 1);
	add_to_blob_cached (image, (char*) "", 1, NULL, 0);
	/* import tables... */
	mono_image_add_stream_data (&image->code, (char*)entrycode, sizeof (entrycode));
	image->iat_offset = mono_image_add_stream_zero (&image->code, 8); /* two IAT entries */
	image->idt_offset = mono_image_add_stream_zero (&image->code, 2 * sizeof (MonoIDT)); /* two IDT entries */
	image->imp_names_offset = mono_image_add_stream_zero (&image->code, 2); /* flags for name entry */
	mono_image_add_stream_data (&image->code, "_CorExeMain", 12);
	mono_image_add_stream_data (&image->code, "mscoree.dll", 12);
	image->ilt_offset = mono_image_add_stream_zero (&image->code, 8); /* two ILT entries */
	stream_data_align (&image->code);

	image->cli_header_offset = mono_image_add_stream_zero (&image->code, sizeof (MonoCLIHeader));

	for (i=0; i < MONO_TABLE_NUM; ++i) {
		image->tables [i].next_idx = 1;
		image->tables [i].columns = table_sizes [i];
	}

	image->image.assembly = (MonoAssembly*)assembly;
	image->run = assembly->run;
	image->save = assembly->save;
	image->pe_kind = 0x1; /* ILOnly */
	image->machine = 0x14c; /* I386 */
	
	mono_profiler_module_loaded (&image->image, MONO_PROFILE_OK);

	return image;
}