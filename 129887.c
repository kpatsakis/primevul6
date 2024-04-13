guint32
mono_declsec_flags_from_method (MonoMethod *method)
{
	if (method->flags & METHOD_ATTRIBUTE_HAS_SECURITY) {
		/* FIXME: No cache (for the moment) */
		guint32 idx = mono_method_get_index (method);
		idx <<= MONO_HAS_DECL_SECURITY_BITS;
		idx |= MONO_HAS_DECL_SECURITY_METHODDEF;
		return mono_declsec_get_flags (method->klass->image, idx);
	}
	return 0;