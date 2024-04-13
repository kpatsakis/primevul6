mono_custom_attrs_from_module (MonoImage *image)
{
	guint32 idx;
	
	if (image->dynamic)
		return lookup_custom_attr (image, image);
	idx = 1; /* there is only one module */
	idx <<= MONO_CUSTOM_ATTR_BITS;
	idx |= MONO_CUSTOM_ATTR_MODULE;
	return mono_custom_attrs_from_index (image, idx);
}