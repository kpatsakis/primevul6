stack_pop (VerifyContext *ctx)
{
	ILStackDesc *ret;
	g_assert (ctx->eval.size > 0);	
	ret = ctx->eval.stack + --ctx->eval.size;
	if ((ret->stype & UNINIT_THIS_MASK) == UNINIT_THIS_MASK)
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Found use of uninitialized 'this ptr' ref at 0x%04x", ctx->ip_offset));
	return ret;
}