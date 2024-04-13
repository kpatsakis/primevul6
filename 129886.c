mono_reflection_get_custom_attrs (MonoObject *obj)
{
	return mono_reflection_get_custom_attrs_by_type (obj, NULL);
}