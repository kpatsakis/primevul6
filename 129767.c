static gboolean
recursive_mark_constraint_args (MonoBitSet *used_args, MonoGenericContainer *gc, MonoType *type)
{
	int idx;
	MonoClass **constraints;
	MonoGenericParamInfo *param_info;

	g_assert (mono_type_is_generic_argument (type));

	idx = mono_type_get_generic_param_num (type);
	if (mono_bitset_test_fast (used_args, idx))
		return FALSE;

	mono_bitset_set_fast (used_args, idx);
	param_info = mono_generic_container_get_param_info (gc, idx);

	if (!param_info->constraints)
		return TRUE;

	for (constraints = param_info->constraints; *constraints; ++constraints) {
		MonoClass *ctr = *constraints;
		MonoType *constraint_type = &ctr->byval_arg;

		if (mono_type_is_generic_argument (constraint_type) && !recursive_mark_constraint_args (used_args, gc, constraint_type))
			return FALSE;
	}
	return TRUE;