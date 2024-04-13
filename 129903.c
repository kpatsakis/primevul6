property_encode_signature (MonoDynamicImage *assembly, MonoReflectionPropertyBuilder *fb)
{
	SigBuffer buf;
	guint32 nparams = 0;
	MonoReflectionMethodBuilder *mb = fb->get_method;
	MonoReflectionMethodBuilder *smb = fb->set_method;
	guint32 idx, i;

	if (mb && mb->parameters)
		nparams = mono_array_length (mb->parameters);
	if (!mb && smb && smb->parameters)
		nparams = mono_array_length (smb->parameters) - 1;
	sigbuffer_init (&buf, 32);
	if (fb->call_conv & 0x20)
		sigbuffer_add_byte (&buf, 0x28);
	else
		sigbuffer_add_byte (&buf, 0x08);
	sigbuffer_add_value (&buf, nparams);
	if (mb) {
		encode_reflection_type (assembly, (MonoReflectionType*)mb->rtype, &buf);
		for (i = 0; i < nparams; ++i) {
			MonoReflectionType *pt = mono_array_get (mb->parameters, MonoReflectionType*, i);
			encode_reflection_type (assembly, pt, &buf);
		}
	} else if (smb && smb->parameters) {
		/* the property type is the last param */
		encode_reflection_type (assembly, mono_array_get (smb->parameters, MonoReflectionType*, nparams), &buf);
		for (i = 0; i < nparams; ++i) {
			MonoReflectionType *pt = mono_array_get (smb->parameters, MonoReflectionType*, i);
			encode_reflection_type (assembly, pt, &buf);
		}
	} else {
		encode_reflection_type (assembly, (MonoReflectionType*)fb->type, &buf);
	}

	idx = sigbuffer_add_to_blob_cached (assembly, &buf);
	sigbuffer_free (&buf);
	return idx;
}