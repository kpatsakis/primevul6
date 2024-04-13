mono_class_has_default_constructor (MonoClass *klass)
{
	MonoMethod *method;
	int i;

	mono_class_setup_methods (klass);
	if (klass->exception_type)
		return FALSE;

	for (i = 0; i < klass->method.count; ++i) {
		method = klass->methods [i];
		if (mono_method_is_constructor (method) &&
			mono_method_signature (method) &&
			mono_method_signature (method)->param_count == 0 &&
			(method->flags & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK) == METHOD_ATTRIBUTE_PUBLIC)
			return TRUE;
	}
	return FALSE;
}