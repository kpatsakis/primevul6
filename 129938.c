MonoBoolean
mono_declsec_get_demands (MonoMethod *method, MonoDeclSecurityActions* demands)
{
	guint32 mask = MONO_DECLSEC_FLAG_DEMAND | MONO_DECLSEC_FLAG_NONCAS_DEMAND | 
		MONO_DECLSEC_FLAG_DEMAND_CHOICE;
	MonoBoolean result = FALSE;
	guint32 flags;

	/* quick exit if no declarative security is present in the metadata */
	if (!method->klass->image->tables [MONO_TABLE_DECLSECURITY].rows)
		return FALSE;

	/* we want the original as the wrapper is "free" of the security informations */
	if (method->wrapper_type == MONO_WRAPPER_MANAGED_TO_NATIVE || method->wrapper_type == MONO_WRAPPER_MANAGED_TO_MANAGED) {
		method = mono_marshal_method_from_wrapper (method);
		if (!method)
			return FALSE;
	}

	/* First we look for method-level attributes */
	if (method->flags & METHOD_ATTRIBUTE_HAS_SECURITY) {
		mono_class_init (method->klass);
		memset (demands, 0, sizeof (MonoDeclSecurityActions));

		result = mono_declsec_get_method_demands_params (method, demands, 
			SECURITY_ACTION_DEMAND, SECURITY_ACTION_NONCASDEMAND, SECURITY_ACTION_DEMANDCHOICE);
	}

	/* Here we use (or create) the class declarative cache to look for demands */
	flags = mono_declsec_flags_from_class (method->klass);
	if (flags & mask) {
		if (!result) {
			mono_class_init (method->klass);
			memset (demands, 0, sizeof (MonoDeclSecurityActions));
		}
		result |= mono_declsec_get_class_demands_params (method->klass, demands, 
			SECURITY_ACTION_DEMAND, SECURITY_ACTION_NONCASDEMAND, SECURITY_ACTION_DEMANDCHOICE);
	}

	/* The boolean return value is used as a shortcut in case nothing needs to
	   be generated (e.g. LinkDemand[Choice] and InheritanceDemand[Choice]) */
	return result;