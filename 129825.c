do_static_branch (VerifyContext *ctx, int delta)
{
	int target = ctx->ip_offset + delta;
	if (target < 0 || target >= ctx->code_size) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("branch target out of code at 0x%04x", ctx->ip_offset));
		return;
	}

	switch (is_valid_branch_instruction (ctx->header, ctx->ip_offset, target)) {
	case 1:
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Branch target escapes out of exception block at 0x%04x", ctx->ip_offset));
		break;
	case 2:
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Branch target escapes out of exception block at 0x%04x", ctx->ip_offset));
		break;
	}

	ctx->target = target;
}