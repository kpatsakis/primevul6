mono_method_is_constructor (MonoMethod *method) 
{
	return ((method->flags & CTOR_REQUIRED_FLAGS) == CTOR_REQUIRED_FLAGS &&
			!(method->flags & CTOR_INVALID_FLAGS) &&
			!strcmp (".ctor", method->name));
}