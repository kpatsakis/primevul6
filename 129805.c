mono_custom_attrs_from_method (MonoMethod *method)
{
	guint32 idx;

	/*
	 * An instantiated method has the same cattrs as the generic method definition.
	 *
	 * LAMESPEC: The .NET SRE throws an exception for instantiations of generic method builders
	 *           Note that this stanza is not necessary for non-SRE types, but it's a micro-optimization
	 */
	if (method->is_inflated)
		method = ((MonoMethodInflated *) method)->declaring;
	
	if (method->dynamic || method->klass->image->dynamic)
		return lookup_custom_attr (method->klass->image, method);

	if (!method->token)
		/* Synthetic methods */
		return NULL;

	idx = mono_method_get_index (method);
	idx <<= MONO_CUSTOM_ATTR_BITS;
	idx |= MONO_CUSTOM_ATTR_METHODDEF;
	return mono_custom_attrs_from_index (method->klass->image, idx);
}