MonoType*
mono_reflection_type_get_handle (MonoReflectionType* ref)
{
	if (!ref)
		return NULL;
	return ref->type;