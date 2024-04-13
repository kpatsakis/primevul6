GSList*
mono_method_verify (MonoMethod *method, int level)
{
	/* The verifier was disabled at compile time */
	return NULL;