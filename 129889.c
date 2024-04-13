do_leave (VerifyContext *ctx, int delta)
{
	int target = ((gint32)ctx->ip_offset) + delta;
	if (target >= ctx->code_size || target < 0)
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Branch target out of code at 0x%04x", ctx->ip_offset));

	if (!is_correct_leave (ctx->header, ctx->ip_offset, target))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Leave not allowed in finally block at 0x%04x", ctx->ip_offset));
	ctx->eval.size = 0;
}