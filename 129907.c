do_ldobj_value (VerifyContext *ctx, int token) 
{
	ILStackDesc *value;
	MonoType *type = get_boxable_mono_type (ctx, token, "ldobj");
	CLEAR_PREFIX (ctx, PREFIX_UNALIGNED | PREFIX_VOLATILE);

	if (!type)
		return;

	if (!check_underflow (ctx, 1))
		return;

	value = stack_pop (ctx);
	if (!stack_slot_is_managed_pointer (value) 
			&& stack_slot_get_type (value) != TYPE_NATIVE_INT
			&& !(stack_slot_get_type (value) == TYPE_PTR && value->type->type != MONO_TYPE_FNPTR)) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Invalid argument %s to ldobj at 0x%04x", stack_slot_get_name (value), ctx->ip_offset));
		return;
	}

	if (stack_slot_get_type (value) == TYPE_NATIVE_INT)
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Using native pointer to ldobj at 0x%04x", ctx->ip_offset));

	/*We have a byval on the stack, but the comparison must be strict. */
	if (!verify_type_compatibility_full (ctx, type, mono_type_get_type_byval (value->type), TRUE))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid type at stack for ldojb operation at 0x%04x", ctx->ip_offset));

	set_stack_value (ctx, stack_push (ctx), type, FALSE);
}