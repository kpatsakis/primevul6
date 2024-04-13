MonoBoolean
mono_declsec_get_inheritdemands_class (MonoClass *klass, MonoDeclSecurityActions* demands)
{
	MonoBoolean result = FALSE;
	guint32 flags;

	/* quick exit if no declarative security is present in the metadata */
	if (!klass->image->tables [MONO_TABLE_DECLSECURITY].rows)
		return FALSE;

	/* Here we use (or create) the class declarative cache to look for demands */
	flags = mono_declsec_flags_from_class (klass);
	if (flags & (MONO_DECLSEC_FLAG_INHERITANCEDEMAND | MONO_DECLSEC_FLAG_NONCAS_INHERITANCEDEMAND | MONO_DECLSEC_FLAG_INHERITANCEDEMAND_CHOICE)) {
		mono_class_init (klass);
		memset (demands, 0, sizeof (MonoDeclSecurityActions));

		result |= mono_declsec_get_class_demands_params (klass, demands, 
			SECURITY_ACTION_INHERITDEMAND, SECURITY_ACTION_NONCASINHERITANCE, SECURITY_ACTION_INHERITDEMANDCHOICE);
	}

	return result;