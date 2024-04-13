mono_type_is_generic_argument (MonoType *type)
{
	return type->type == MONO_TYPE_VAR || type->type == MONO_TYPE_MVAR;
}