mono_image_get_methodref_token_for_methodbuilder (MonoDynamicImage *assembly, MonoReflectionMethodBuilder *method)
{
	guint32 token, parent, sig;
	ReflectionMethodBuilder rmb;
	char *name;
	MonoReflectionTypeBuilder *tb = (MonoReflectionTypeBuilder *)method->type;
	
	token = GPOINTER_TO_UINT (g_hash_table_lookup (assembly->handleref, method));
	if (token)
		return token;

	name = mono_string_to_utf8 (method->name);
	reflection_methodbuilder_from_method_builder (&rmb, method);

	/*
	 * A methodref signature can't contain an unmanaged calling convention.
	 * Since some flags are encoded as part of call_conv, we need to check against it.
	*/
	if ((rmb.call_conv & ~0x60) != MONO_CALL_DEFAULT && (rmb.call_conv & ~0x60) != MONO_CALL_VARARG)
		rmb.call_conv = (rmb.call_conv & 0x60) | MONO_CALL_DEFAULT;

	sig = method_builder_encode_signature (assembly, &rmb);

	if (tb->generic_params)
		parent = create_generic_typespec (assembly, tb);
	else
		parent = mono_image_typedef_or_ref (assembly, mono_reflection_type_get_handle ((MonoReflectionType*)rmb.type));

	token = mono_image_add_memberef_row (assembly, parent, name, sig);

	g_free (name);
	g_hash_table_insert (assembly->handleref, method, GUINT_TO_POINTER(token));
	return token;
}