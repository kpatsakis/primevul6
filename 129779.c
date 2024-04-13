mono_type_get_type_byref (MonoType *type)
{
	if (type->byref)
		return type;
	return &mono_class_from_mono_type (type)->this_arg;
}