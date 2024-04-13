MonoBoolean
mono_declsec_get_inheritdemands_method (MonoMethod *method, MonoDeclSecurityActions* demands)
{
	/* quick exit if no declarative security is present in the metadata */
	if (!method->klass->image->tables [MONO_TABLE_DECLSECURITY].rows)
		return FALSE;

	/* we want the original as the wrapper is "free" of the security informations */
	if (method->wrapper_type == MONO_WRAPPER_MANAGED_TO_NATIVE || method->wrapper_type == MONO_WRAPPER_MANAGED_TO_MANAGED) {
		method = mono_marshal_method_from_wrapper (method);
		if (!method)
			return FALSE;
	}

	if (method->flags & METHOD_ATTRIBUTE_HAS_SECURITY) {
		mono_class_init (method->klass);
		memset (demands, 0, sizeof (MonoDeclSecurityActions));

		return mono_declsec_get_method_demands_params (method, demands, 
			SECURITY_ACTION_INHERITDEMAND, SECURITY_ACTION_NONCASINHERITANCE, SECURITY_ACTION_INHERITDEMANDCHOICE);
	}
	return FALSE;