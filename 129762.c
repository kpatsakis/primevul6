mono_custom_attrs_construct_by_type (MonoCustomAttrInfo *cinfo, MonoClass *attr_klass)
{
	MonoArray *result;
	MonoObject *attr;
	int i, n;

	n = 0;
	for (i = 0; i < cinfo->num_attrs; ++i) {
		if (mono_class_is_assignable_from (attr_klass, cinfo->attrs [i].ctor->klass))
			n ++;
	}

	result = mono_array_new_cached (mono_domain_get (), mono_defaults.attribute_class, n);
	n = 0;
	for (i = 0; i < cinfo->num_attrs; ++i) {
		if (mono_class_is_assignable_from (attr_klass, cinfo->attrs [i].ctor->klass)) {
			attr = create_custom_attr (cinfo->image, cinfo->attrs [i].ctor, cinfo->attrs [i].data, cinfo->attrs [i].data_size);
			mono_array_setref (result, n, attr);
			n ++;
		}
	}
	return result;
}