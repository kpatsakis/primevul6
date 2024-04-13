mono_type_is_value_type (MonoType *type, const char *namespace, const char *name)
{
	return type->type == MONO_TYPE_VALUETYPE &&
		!strcmp (namespace, type->data.klass->name_space) &&
		!strcmp (name, type->data.klass->name);
}