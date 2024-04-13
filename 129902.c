do_store_indirect (VerifyContext *ctx, int opcode)
{
	ILStackDesc *addr, *val;
	CLEAR_PREFIX (ctx, PREFIX_UNALIGNED | PREFIX_VOLATILE);

	if (!check_underflow (ctx, 2))
		return;

	val = stack_pop (ctx);
	addr = stack_pop (ctx);	

	check_unmanaged_pointer (ctx, addr);

	if (!stack_slot_is_managed_pointer (addr) && stack_slot_get_type (addr) != TYPE_PTR) {
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid non-pointer argument to stind at 0x%04x", ctx->ip_offset));
		return;
	}

	if (stack_slot_is_managed_mutability_pointer (addr)) {
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot use a readonly pointer with stind at 0x%04x", ctx->ip_offset));
		return;
	}

	if (!verify_type_compatibility_full (ctx, mono_type_from_opcode (opcode), mono_type_get_type_byval (addr->type), TRUE))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid addr type at stack for stind 0x%x operation at 0x%04x", opcode, ctx->ip_offset));

	if (!verify_stack_type_compatibility (ctx, mono_type_from_opcode (opcode), val))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid value type at stack for stind 0x%x operation at 0x%04x", opcode, ctx->ip_offset));
}