static void fastrpc_channel_ctx_get(struct fastrpc_channel_ctx *cctx)
{
	kref_get(&cctx->refcount);
}