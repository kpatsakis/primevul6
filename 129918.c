mono_custom_attrs_from_assembly (MonoAssembly *assembly)
{
	guint32 idx;
	
	if (assembly->image->dynamic)
		return lookup_custom_attr (assembly->image, assembly);
	idx = 1; /* there is only one assembly */
	idx <<= MONO_CUSTOM_ATTR_BITS;
	idx |= MONO_CUSTOM_ATTR_ASSEMBLY;
	return mono_custom_attrs_from_index (assembly->image, idx);
}