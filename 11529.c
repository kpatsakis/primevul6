static void fastrpc_context_get(struct fastrpc_invoke_ctx *ctx)
{
	kref_get(&ctx->refcount);
}