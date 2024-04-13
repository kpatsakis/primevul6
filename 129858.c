mono_reflection_get_custom_attrs_by_type (MonoObject *obj, MonoClass *attr_klass)
{
	MonoArray *result;
	MonoCustomAttrInfo *cinfo;

	cinfo = mono_reflection_get_custom_attrs_info (obj);
	if (cinfo) {
		if (attr_klass)
			result = mono_custom_attrs_construct_by_type (cinfo, attr_klass);
		else
			result = mono_custom_attrs_construct (cinfo);
		if (!cinfo->cached)
			mono_custom_attrs_free (cinfo);
	} else {
		if (mono_loader_get_last_error ())
			return NULL;
		result = mono_array_new_cached (mono_domain_get (), mono_defaults.attribute_class, 0);
	}

	return result;
}