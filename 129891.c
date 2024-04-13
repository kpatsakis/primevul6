gboolean
mono_verifier_is_enabled_for_method (MonoMethod *method)
{
	/* The verifier was disabled at compile time */
	return FALSE;