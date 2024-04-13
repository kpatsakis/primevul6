gboolean
mono_verifier_is_enabled_for_method (MonoMethod *method)
{
	return mono_verifier_is_enabled_for_class (method->klass) && method->wrapper_type == MONO_WRAPPER_NONE;