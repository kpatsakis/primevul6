static gboolean
verify_generic_parameters (MonoClass *class)
{
	int i;
	MonoGenericContainer *gc = class->generic_container;
	MonoBitSet *used_args = mono_bitset_new (gc->type_argc, 0);

	for (i = 0; i < gc->type_argc; ++i) {
		MonoGenericParamInfo *param_info = mono_generic_container_get_param_info (gc, i);
		MonoClass **constraints;

		if (!param_info->constraints)
			continue;

		mono_bitset_clear_all (used_args);
		mono_bitset_set_fast (used_args, i);

		for (constraints = param_info->constraints; *constraints; ++constraints) {
			MonoClass *ctr = *constraints;
			MonoType *constraint_type = &ctr->byval_arg;

			if (!mono_type_is_valid_type_in_context (constraint_type, &gc->context))
				goto fail;

			if (mono_type_is_generic_argument (constraint_type) && !recursive_mark_constraint_args (used_args, gc, constraint_type))
				goto fail;
		}
	}
	mono_bitset_free (used_args);
	return TRUE;

fail:
	mono_bitset_free (used_args);
	return FALSE;