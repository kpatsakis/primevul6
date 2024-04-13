gboolean
mono_verifier_is_enabled_for_image (MonoImage *image)
{
	/* The verifier was disabled at compile time */
	return FALSE;