GSList*
mono_method_verify_with_current_settings (MonoMethod *method, gboolean skip_visibility)
{
	return mono_method_verify (method, 
			(verifier_mode != MONO_VERIFIER_MODE_STRICT ? MONO_VERIFY_NON_STRICT: 0)
			| (!mono_verifier_is_method_full_trust (method) ? MONO_VERIFY_FAIL_FAST : 0)
			| (skip_visibility ? MONO_VERIFY_SKIP_VISIBILITY : 0));