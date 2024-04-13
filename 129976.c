encode_generic_method_sig (MonoDynamicImage *assembly, MonoGenericContext *context)
{
	SigBuffer buf;
	int i;
	guint32 nparams = context->method_inst->type_argc;
	guint32 idx;

	if (!assembly->save)
		return 0;

	sigbuffer_init (&buf, 32);
	/*
	 * FIXME: vararg, explicit_this, differenc call_conv values...
	 */
	sigbuffer_add_value (&buf, 0xa); /* FIXME FIXME FIXME */
	sigbuffer_add_value (&buf, nparams);

	for (i = 0; i < nparams; i++)
		encode_type (assembly, context->method_inst->type_argv [i], &buf);

	idx = sigbuffer_add_to_blob_cached (assembly, &buf);
	sigbuffer_free (&buf);
	return idx;
}