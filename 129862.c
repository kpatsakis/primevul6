mono_custom_attrs_from_property (MonoClass *klass, MonoProperty *property)
{
	guint32 idx;
	
	if (klass->image->dynamic) {
		property = mono_metadata_get_corresponding_property_from_generic_type_definition (property);
		return lookup_custom_attr (klass->image, property);
	}
	idx = find_property_index (klass, property);
	idx <<= MONO_CUSTOM_ATTR_BITS;
	idx |= MONO_CUSTOM_ATTR_PROPERTY;
	return mono_custom_attrs_from_index (klass->image, idx);
}