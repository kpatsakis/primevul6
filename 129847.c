mono_type_get_stack_name (MonoType *type)
{
	return type_names [get_stack_type (type) & TYPE_MASK];
}