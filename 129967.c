mono_custom_attrs_from_field (MonoClass *klass, MonoClassField *field)
{
	guint32 idx;
	if (klass->image->dynamic) {
		field = mono_metadata_get_corresponding_field_from_generic_type_definition (field);
		return lookup_custom_attr (klass->image, field);
	}
	idx = find_field_index (klass, field);
	idx <<= MONO_CUSTOM_ATTR_BITS;
	idx |= MONO_CUSTOM_ATTR_FIELDDEF;
	return mono_custom_attrs_from_index (klass->image, idx);
}