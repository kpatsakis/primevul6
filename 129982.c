mono_image_get_methodbuilder_token (MonoDynamicImage *assembly, MonoReflectionMethodBuilder *mb, gboolean create_methodspec)
{
	guint32 token;

	if (mb->generic_params && create_methodspec) 
		return mono_image_get_methodspec_token_for_generic_method_definition (assembly, mb);

	token = GPOINTER_TO_UINT (mono_g_hash_table_lookup (assembly->handleref_managed, mb));
	if (token)
		return token;

	token = mono_image_get_methodref_token_for_methodbuilder (assembly, mb);
	mono_g_hash_table_insert (assembly->handleref_managed, mb, GUINT_TO_POINTER(token));
	return token;
}