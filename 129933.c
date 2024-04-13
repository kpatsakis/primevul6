gboolean
mono_verifier_class_is_valid_generic_instantiation (MonoClass *class)
{
	return mono_class_is_valid_generic_instantiation (NULL, class);