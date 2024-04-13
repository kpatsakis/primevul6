create_custom_attr_data (MonoImage *image, MonoCustomAttrEntry *cattr)
{
	static MonoMethod *ctor;
	MonoDomain *domain;
	MonoObject *attr;
	void *params [4];

	g_assert (image->assembly);

	if (!ctor)
		ctor = mono_class_get_method_from_name (mono_defaults.customattribute_data_class, ".ctor", 4);

	domain = mono_domain_get ();
	attr = mono_object_new (domain, mono_defaults.customattribute_data_class);
	params [0] = mono_method_get_object (domain, cattr->ctor, NULL);
	params [1] = mono_assembly_get_object (domain, image->assembly);
	params [2] = (gpointer)&cattr->data;
	params [3] = &cattr->data_size;
	mono_runtime_invoke (ctor, attr, params, NULL);
	return attr;
}