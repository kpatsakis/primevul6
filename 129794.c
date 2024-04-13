mono_image_get_array_token (MonoDynamicImage *assembly, MonoReflectionArrayMethod *m)
{
	guint32 nparams, i;
	GList *tmp;
	char *name;
	MonoMethodSignature *sig;
	ArrayMethod *am;
	MonoType *mtype;

	name = mono_string_to_utf8 (m->name);
	nparams = mono_array_length (m->parameters);
	sig = g_malloc0 (MONO_SIZEOF_METHOD_SIGNATURE + sizeof (MonoType*) * nparams);
	sig->hasthis = 1;
	sig->sentinelpos = -1;
	sig->call_convention = reflection_cc_to_file (m->call_conv);
	sig->param_count = nparams;
	sig->ret = m->ret ? mono_reflection_type_get_handle (m->ret): &mono_defaults.void_class->byval_arg;
	mtype = mono_reflection_type_get_handle (m->parent);
	for (i = 0; i < nparams; ++i)
		sig->params [i] = mono_type_array_get_and_resolve (m->parameters, i);

	for (tmp = assembly->array_methods; tmp; tmp = tmp->next) {
		am = tmp->data;
		if (strcmp (name, am->name) == 0 && 
				mono_metadata_type_equal (am->parent, mtype) &&
				mono_metadata_signature_equal (am->sig, sig)) {
			g_free (name);
			g_free (sig);
			m->table_idx = am->token & 0xffffff;
			return am->token;
		}
	}
	am = g_new0 (ArrayMethod, 1);
	am->name = name;
	am->sig = sig;
	am->parent = mtype;
	am->token = mono_image_get_memberref_token (assembly, am->parent, name,
		method_encode_signature (assembly, sig));
	assembly->array_methods = g_list_prepend (assembly->array_methods, am);
	m->table_idx = am->token & 0xffffff;
	return am->token;
}