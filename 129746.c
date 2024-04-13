do_initobj (VerifyContext *ctx, int token)
{
	ILStackDesc *obj;
	MonoType *stack, *type = get_boxable_mono_type (ctx, token, "initobj");
	if (!type)
		return;

	if (!check_underflow (ctx, 1))
		return;

	obj = stack_pop (ctx);

	if (!stack_slot_is_managed_pointer (obj)) 
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid object address for initobj at 0x%04x", ctx->ip_offset));

	if (stack_slot_is_managed_mutability_pointer (obj))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot use a readonly pointer with initobj at 0x%04x", ctx->ip_offset));

	stack = mono_type_get_type_byval (obj->type);
	if (MONO_TYPE_IS_REFERENCE (stack)) {
		if (!verify_type_compatibility (ctx, stack, type)) 
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Type token of initobj not compatible with value on stack at 0x%04x", ctx->ip_offset));
		else if (IS_STRICT_MODE (ctx) && !mono_metadata_type_equal (type, stack)) 
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Type token of initobj not compatible with value on stack at 0x%04x", ctx->ip_offset));
	} else if (!verify_type_compatibility (ctx, stack, type)) {
		char *expected_name = mono_type_full_name (type);
		char *stack_name = mono_type_full_name (stack);

		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Initobj %s not compatible with value on stack %s at 0x%04x", expected_name, stack_name, ctx->ip_offset));
		g_free (expected_name);
		g_free (stack_name);
	}
}