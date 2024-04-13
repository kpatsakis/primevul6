do_ldelem (VerifyContext *ctx, int opcode, int token)
{
#define IS_ONE_OF2(T, A, B) (T == A || T == B)
	ILStackDesc *index, *array;
	MonoType *type;
	if (!check_underflow (ctx, 2))
		return;

	if (opcode == CEE_LDELEM) {
		if (!(type = verifier_load_type (ctx, token, "ldelem.any"))) {
			ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Type (0x%08x) not found at 0x%04x", token, ctx->ip_offset));
			return;
		}
	} else {
		type = mono_type_from_opcode (opcode);
	}

	index = stack_pop (ctx);
	array = stack_pop (ctx);

	if (stack_slot_get_type (index) != TYPE_I4 && stack_slot_get_type (index) != TYPE_NATIVE_INT)
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Index type(%s) for ldelem.X is not an int or a native int at 0x%04x", stack_slot_get_name (index), ctx->ip_offset));

	if (!stack_slot_is_null_literal (array)) {
		if (stack_slot_get_type (array) != TYPE_COMPLEX || array->type->type != MONO_TYPE_SZARRAY)
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid array type(%s) for ldelem.X at 0x%04x", stack_slot_get_name (array), ctx->ip_offset));
		else {
			if (opcode == CEE_LDELEM_REF) {
				if (array->type->data.klass->valuetype)
					CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid array type is not a reference type for ldelem.ref 0x%04x", ctx->ip_offset));
				type = &array->type->data.klass->byval_arg;
			} else {
				MonoType *candidate = &array->type->data.klass->byval_arg;
				if (IS_STRICT_MODE (ctx)) {
					MonoType *underlying_type = mono_type_get_underlying_type_any (type);
					MonoType *underlying_candidate = mono_type_get_underlying_type_any (candidate);
					if ((IS_ONE_OF2 (underlying_type->type, MONO_TYPE_I4, MONO_TYPE_U4) && IS_ONE_OF2 (underlying_candidate->type, MONO_TYPE_I, MONO_TYPE_U)) ||
						(IS_ONE_OF2 (underlying_candidate->type, MONO_TYPE_I4, MONO_TYPE_U4) && IS_ONE_OF2 (underlying_type->type, MONO_TYPE_I, MONO_TYPE_U)))
						CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid array type on stack for ldelem.X at 0x%04x", ctx->ip_offset));
				}
				if (!verify_type_compatibility_full (ctx, type, candidate, TRUE))
					CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid array type on stack for ldelem.X at 0x%04x", ctx->ip_offset));
			}
		}
	}

	set_stack_value (ctx, stack_push (ctx), type, FALSE);
#undef IS_ONE_OF2
}