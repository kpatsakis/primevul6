mono_custom_attrs_data_construct (MonoCustomAttrInfo *cinfo)
{
	MonoArray *result;
	MonoObject *attr;
	int i;
	
	result = mono_array_new (mono_domain_get (), mono_defaults.customattribute_data_class, cinfo->num_attrs);
	for (i = 0; i < cinfo->num_attrs; ++i) {
		attr = create_custom_attr_data (cinfo->image, &cinfo->attrs [i]);
		mono_array_setref (result, i, attr);
	}
	return result;
}