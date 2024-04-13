static void fastrpc_context_put(struct fastrpc_invoke_ctx *ctx)
{
	kref_put(&ctx->refcount, fastrpc_context_free);
}