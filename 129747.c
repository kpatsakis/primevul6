GSList*
mono_method_verify_with_current_settings (MonoMethod *method, gboolean skip_visibility)
{
	/* The verifier was disabled at compile time */
	return NULL;