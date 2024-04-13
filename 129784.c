mono_reflection_get_custom_attrs_data (MonoObject *obj)
{
	MonoArray *result;
	MonoCustomAttrInfo *cinfo;

	cinfo = mono_reflection_get_custom_attrs_info (obj);
	if (cinfo) {
		result = mono_custom_attrs_data_construct (cinfo);
		if (!cinfo->cached)
			mono_custom_attrs_free (cinfo);
	} else
		result = mono_array_new (mono_domain_get (), mono_defaults.customattribute_data_class, 0);

	return result;
}