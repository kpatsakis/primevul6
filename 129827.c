method_builder_encode_signature (MonoDynamicImage *assembly, ReflectionMethodBuilder *mb)
{
	/*
	 * FIXME: reuse code from method_encode_signature().
	 */
	SigBuffer buf;
	int i;
	guint32 nparams =  mb->parameters ? mono_array_length (mb->parameters): 0;
	guint32 ngparams = mb->generic_params ? mono_array_length (mb->generic_params): 0;
	guint32 notypes = mb->opt_types ? mono_array_length (mb->opt_types): 0;
	guint32 idx;

	sigbuffer_init (&buf, 32);
	/* LAMESPEC: all the call conv spec is foobared */
	idx = mb->call_conv & 0x60; /* has-this, explicit-this */
	if (mb->call_conv & 2)
		idx |= 0x5; /* vararg */
	if (!(mb->attrs & METHOD_ATTRIBUTE_STATIC))
		idx |= 0x20; /* hasthis */
	if (ngparams)
		idx |= 0x10; /* generic */
	sigbuffer_add_byte (&buf, idx);
	if (ngparams)
		sigbuffer_add_value (&buf, ngparams);
	sigbuffer_add_value (&buf, nparams + notypes);
	encode_custom_modifiers (assembly, mb->return_modreq, mb->return_modopt, &buf);
	encode_reflection_type (assembly, mb->rtype, &buf);
	for (i = 0; i < nparams; ++i) {
		MonoArray *modreq = NULL;
		MonoArray *modopt = NULL;
		MonoReflectionType *pt;

		if (mb->param_modreq && (i < mono_array_length (mb->param_modreq)))
			modreq = mono_array_get (mb->param_modreq, MonoArray*, i);
		if (mb->param_modopt && (i < mono_array_length (mb->param_modopt)))
			modopt = mono_array_get (mb->param_modopt, MonoArray*, i);
		encode_custom_modifiers (assembly, modreq, modopt, &buf);
		pt = mono_array_get (mb->parameters, MonoReflectionType*, i);
		encode_reflection_type (assembly, pt, &buf);
	}
	if (notypes)
		sigbuffer_add_byte (&buf, MONO_TYPE_SENTINEL);
	for (i = 0; i < notypes; ++i) {
		MonoReflectionType *pt;

		pt = mono_array_get (mb->opt_types, MonoReflectionType*, i);
		encode_reflection_type (assembly, pt, &buf);
	}

	idx = sigbuffer_add_to_blob_cached (assembly, &buf);
	sigbuffer_free (&buf);
	return idx;
}