recursive_boxed_constraint_type_check (VerifyContext *ctx, MonoType *type, MonoClass *constraint_class, int recursion_level)
{
	MonoType *constraint_type = &constraint_class->byval_arg;
	if (recursion_level <= 0)
		return FALSE;

	if (verify_type_compatibility_full (ctx, type, mono_type_get_type_byval (constraint_type), FALSE))
		return TRUE;

	if (mono_type_is_generic_argument (constraint_type)) {
		MonoGenericParam *param = get_generic_param (ctx, constraint_type);
		MonoClass **class;
		if (!param)
			return FALSE;
		for (class = mono_generic_param_info (param)->constraints; class && *class; ++class) {
			if (recursive_boxed_constraint_type_check (ctx, type, *class, recursion_level - 1))
				return TRUE;
		}
	}
	return FALSE;
}