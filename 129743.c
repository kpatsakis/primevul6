mono_get_object_from_blob (MonoDomain *domain, MonoType *type, const char *blob)
{
	void *retval;
	MonoClass *klass;
	MonoObject *object;
	MonoType *basetype = type;

	if (!blob)
		return NULL;
	
	klass = mono_class_from_mono_type (type);
	if (klass->valuetype) {
		object = mono_object_new (domain, klass);
		retval = ((gchar *) object + sizeof (MonoObject));
		if (klass->enumtype)
			basetype = mono_class_enum_basetype (klass);
	} else {
		retval = &object;
	}
			
	if (!mono_get_constant_value_from_blob (domain, basetype->type,  blob, retval))
		return object;
	else
		return NULL;
}