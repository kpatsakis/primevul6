stack_peek (VerifyContext *ctx, int distance)
{
	g_assert (ctx->eval.size - distance > 0);
	return ctx->eval.stack + (ctx->eval.size - 1 - distance);
}