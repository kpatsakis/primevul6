static void fastrpc_channel_ctx_free(struct kref *ref)
{
	struct fastrpc_channel_ctx *cctx;

	cctx = container_of(ref, struct fastrpc_channel_ctx, refcount);

	kfree(cctx);
}