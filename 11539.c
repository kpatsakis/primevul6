static void fastrpc_context_free(struct kref *ref)
{
	struct fastrpc_invoke_ctx *ctx;
	struct fastrpc_channel_ctx *cctx;
	unsigned long flags;
	int i;

	ctx = container_of(ref, struct fastrpc_invoke_ctx, refcount);
	cctx = ctx->cctx;

	for (i = 0; i < ctx->nscalars; i++)
		fastrpc_map_put(ctx->maps[i]);

	if (ctx->buf)
		fastrpc_buf_free(ctx->buf);

	spin_lock_irqsave(&cctx->lock, flags);
	idr_remove(&cctx->ctx_idr, ctx->ctxid >> 4);
	spin_unlock_irqrestore(&cctx->lock, flags);

	kfree(ctx->maps);
	kfree(ctx->olaps);
	kfree(ctx);

	fastrpc_channel_ctx_put(cctx);
}