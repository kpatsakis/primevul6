gboolean
mono_verifier_is_method_full_trust (MonoMethod *method)
{
	return mono_verifier_is_class_full_trust (method->klass);