do_load_indirect (VerifyContext *ctx, int opcode)
{
	ILStackDesc *value;
	CLEAR_PREFIX (ctx, PREFIX_UNALIGNED | PREFIX_VOLATILE);

	if (!check_underflow (ctx, 1))
		return;
	
	value = stack_pop (ctx);
	if (!stack_slot_is_managed_pointer (value)) {
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Load indirect not using a manager pointer at 0x%04x", ctx->ip_offset));
		set_stack_value (ctx, stack_push (ctx), mono_type_from_opcode (opcode), FALSE);
		return;
	}

	if (opcode == CEE_LDIND_REF) {
		if (stack_slot_get_underlying_type (value) != TYPE_COMPLEX || mono_class_from_mono_type (value->type)->valuetype)
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid type at stack for ldind_ref expected object byref operation at 0x%04x", ctx->ip_offset));
		set_stack_value (ctx, stack_push (ctx), mono_type_get_type_byval (value->type), FALSE);
	} else {
		if (!verify_type_compatibility_full (ctx, mono_type_from_opcode (opcode), mono_type_get_type_byval (value->type), TRUE))
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid type at stack for ldind 0x%x operation at 0x%04x", opcode, ctx->ip_offset));
		set_stack_value (ctx, stack_push (ctx), mono_type_from_opcode (opcode), FALSE);
	}
}