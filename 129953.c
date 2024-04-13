do_ckfinite (VerifyContext *ctx)
{
	ILStackDesc *top;
	if (!check_underflow (ctx, 1))
		return;

	top = stack_pop (ctx);

	if (stack_slot_get_underlying_type (top) != TYPE_R8)
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Expected float32 or float64 on stack for ckfinit but found %s at 0x%04x", stack_slot_get_name (top), ctx->ip_offset));	
	stack_push_stack_val (ctx, top);
}