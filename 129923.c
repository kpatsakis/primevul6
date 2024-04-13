gboolean
mono_verifier_is_enabled_for_image (MonoImage *image)
{
	return verify_all || verifier_mode > MONO_VERIFIER_MODE_OFF;