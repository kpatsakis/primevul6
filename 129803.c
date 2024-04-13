do_ldelema (VerifyContext *ctx, int klass_token)
{
	ILStackDesc *index, *array, *res;
	MonoType *type = get_boxable_mono_type (ctx, klass_token, "ldelema");
	gboolean valid; 

	if (!type)
		return;

	if (!check_underflow (ctx, 2))
		return;

	index = stack_pop (ctx);
	array = stack_pop (ctx);

	if (stack_slot_get_type (index) != TYPE_I4 && stack_slot_get_type (index) != TYPE_NATIVE_INT)
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Index type(%s) for ldelema is not an int or a native int at 0x%04x", stack_slot_get_name (index), ctx->ip_offset));

	if (!stack_slot_is_null_literal (array)) {
		if (stack_slot_get_type (array) != TYPE_COMPLEX || array->type->type != MONO_TYPE_SZARRAY)
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid array type(%s) for ldelema at 0x%04x", stack_slot_get_name (array), ctx->ip_offset));
		else {
			if (get_stack_type (type) == TYPE_I4 || get_stack_type (type) == TYPE_NATIVE_INT) {
					valid = verify_type_compatibility_full (ctx, type, &array->type->data.klass->byval_arg, TRUE);
			} else {
				valid = mono_metadata_type_equal (type, &array->type->data.klass->byval_arg);
			}
			if (!valid)
				CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid array type on stack for ldelema at 0x%04x", ctx->ip_offset));
		}
	}

	res = stack_push (ctx);
	set_stack_value (ctx, res, type, TRUE);
	if (ctx->prefix_set & PREFIX_READONLY) {
		ctx->prefix_set &= ~PREFIX_READONLY;
		res->stype |= CMMP_MASK;
	}
}