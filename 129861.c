is_compatible_boxed_valuetype (VerifyContext *ctx, MonoType *type, MonoType *candidate, ILStackDesc *stack, gboolean strict)
{
	if (!stack_slot_is_boxed_value (stack))
		return FALSE;
	if (type->byref || candidate->byref)
		return FALSE;

	if (mono_type_is_generic_argument (candidate)) {
		MonoGenericParam *param = get_generic_param (ctx, candidate);
		MonoClass **class;
		if (!param)
			return FALSE;

		for (class = mono_generic_param_info (param)->constraints; class && *class; ++class) {
			/*256 should be enough since there can't be more than 255 generic arguments.*/
			if (recursive_boxed_constraint_type_check (ctx, type, *class, 256))
				return TRUE;
		}
	}

	if (mono_type_is_generic_argument (type))
		return FALSE;

	if (!strict)
		return TRUE;

	return MONO_TYPE_IS_REFERENCE (type) && mono_class_is_assignable_from (mono_class_from_mono_type (type), mono_class_from_mono_type (candidate));
}