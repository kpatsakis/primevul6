static void fastrpc_channel_ctx_put(struct fastrpc_channel_ctx *cctx)
{
	kref_put(&cctx->refcount, fastrpc_channel_ctx_free);
}