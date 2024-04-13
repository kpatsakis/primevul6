generic_arguments_respect_constraints (VerifyContext *ctx, MonoGenericContainer *gc, MonoGenericContext *context, MonoGenericInst *ginst)
{
	int i;
	for (i = 0; i < ginst->type_argc; ++i) {
		MonoType *type = ginst->type_argv [i];
		MonoGenericParam *target = mono_generic_container_get_param (gc, i);
		MonoGenericParam *candidate;
		MonoClass *candidate_class;

		if (!mono_type_is_generic_argument (type))
			continue;

		if (!is_valid_type_in_context (ctx, type))
			return FALSE;

		candidate = verifier_get_generic_param_from_type (ctx, type);
		candidate_class = mono_class_from_mono_type (type);

		if (!mono_generic_param_is_constraint_compatible (ctx, target, candidate, candidate_class, context))
			return FALSE;
	}
	return TRUE;
}