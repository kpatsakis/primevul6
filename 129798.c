gboolean
mono_verifier_is_enabled_for_class (MonoClass *klass)
{
	/* The verifier was disabled at compile time */
	return FALSE;