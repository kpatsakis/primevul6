lookup_custom_attr (MonoImage *image, gpointer member)
{
	MonoCustomAttrInfo* res;

	res = mono_image_property_lookup (image, member, MONO_PROP_DYNAMIC_CATTR);

	if (!res)
		return NULL;

	return g_memdup (res, MONO_SIZEOF_CUSTOM_ATTR_INFO + sizeof (MonoCustomAttrEntry) * res->num_attrs);
}