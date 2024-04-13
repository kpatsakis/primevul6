mono_dynamic_image_free (MonoDynamicImage *image)
{
	MonoDynamicImage *di = image;
	GList *list;
	int i;

	if (di->methodspec)
		mono_g_hash_table_destroy (di->methodspec);
	if (di->typespec)
		g_hash_table_destroy (di->typespec);
	if (di->typeref)
		g_hash_table_destroy (di->typeref);
	if (di->handleref)
		g_hash_table_destroy (di->handleref);
	if (di->handleref_managed)
		mono_g_hash_table_destroy (di->handleref_managed);
	if (di->tokens)
		mono_g_hash_table_destroy (di->tokens);
	if (di->generic_def_objects)
		mono_g_hash_table_destroy (di->generic_def_objects);
	if (di->blob_cache) {
		g_hash_table_foreach (di->blob_cache, free_blob_cache_entry, NULL);
		g_hash_table_destroy (di->blob_cache);
	}
	if (di->standalonesig_cache)
		g_hash_table_destroy (di->standalonesig_cache);
	for (list = di->array_methods; list; list = list->next) {
		ArrayMethod *am = (ArrayMethod *)list->data;
		g_free (am->sig);
		g_free (am->name);
		g_free (am);
	}
	g_list_free (di->array_methods);
	if (di->gen_params) {
		for (i = 0; i < di->gen_params->len; i++) {
			GenericParamTableEntry *entry = g_ptr_array_index (di->gen_params, i);
			if (entry->gparam->type.type) {
				MonoGenericParam *param = entry->gparam->type.type->data.generic_param;
				g_free ((char*)mono_generic_param_info (param)->name);
				g_free (param);
			}
			mono_gc_deregister_root ((char*) &entry->gparam);
			g_free (entry);
		}
	 	g_ptr_array_free (di->gen_params, TRUE);
	}
	if (di->token_fixups)
		mono_g_hash_table_destroy (di->token_fixups);
	if (di->method_to_table_idx)
		g_hash_table_destroy (di->method_to_table_idx);
	if (di->field_to_table_idx)
		g_hash_table_destroy (di->field_to_table_idx);
	if (di->method_aux_hash)
		g_hash_table_destroy (di->method_aux_hash);
	if (di->vararg_aux_hash)
		g_hash_table_destroy (di->vararg_aux_hash);
	g_free (di->strong_name);
	g_free (di->win32_res);
	if (di->public_key)
		g_free (di->public_key);

	/*g_print ("string heap destroy for image %p\n", di);*/
	mono_dynamic_stream_reset (&di->sheap);
	mono_dynamic_stream_reset (&di->code);
	mono_dynamic_stream_reset (&di->resources);
	mono_dynamic_stream_reset (&di->us);
	mono_dynamic_stream_reset (&di->blob);
	mono_dynamic_stream_reset (&di->tstream);
	mono_dynamic_stream_reset (&di->guid);
	for (i = 0; i < MONO_TABLE_NUM; ++i) {
		g_free (di->tables [i].values);
	}
}	