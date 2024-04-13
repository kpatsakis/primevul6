gboolean
mono_verifier_verify_class (MonoClass *class)
{
	/* The verifier was disabled at compile time */
	return TRUE;