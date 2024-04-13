mono_image_get_methodspec_token (MonoDynamicImage *assembly, MonoMethod *method)
{
	MonoMethodInflated *imethod;
	guint32 token;
	
	token = GPOINTER_TO_UINT (g_hash_table_lookup (assembly->handleref, method));
	if (token)
		return token;

	g_assert (method->is_inflated);
	imethod = (MonoMethodInflated *) method;

	if (mono_method_signature (imethod->declaring)->generic_param_count) {
		token = method_encode_methodspec (assembly, method);
	} else {
		guint32 sig = method_encode_signature (
			assembly, mono_method_signature (imethod->declaring));
		token = mono_image_get_memberref_token (
			assembly, &method->klass->byval_arg, method->name, sig);
	}

	g_hash_table_insert (assembly->handleref, method, GUINT_TO_POINTER(token));
	return token;
}