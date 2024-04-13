static int fastrpc_rpmsg_callback(struct rpmsg_device *rpdev, void *data,
				  int len, void *priv, u32 addr)
{
	struct fastrpc_channel_ctx *cctx = dev_get_drvdata(&rpdev->dev);
	struct fastrpc_invoke_rsp *rsp = data;
	struct fastrpc_invoke_ctx *ctx;
	unsigned long flags;
	unsigned long ctxid;

	if (len < sizeof(*rsp))
		return -EINVAL;

	ctxid = ((rsp->ctx & FASTRPC_CTXID_MASK) >> 4);

	spin_lock_irqsave(&cctx->lock, flags);
	ctx = idr_find(&cctx->ctx_idr, ctxid);
	spin_unlock_irqrestore(&cctx->lock, flags);

	if (!ctx) {
		dev_err(&rpdev->dev, "No context ID matches response\n");
		return -ENOENT;
	}

	ctx->retval = rsp->retval;
	complete(&ctx->work);

	/*
	 * The DMA buffer associated with the context cannot be freed in
	 * interrupt context so schedule it through a worker thread to
	 * avoid a kernel BUG.
	 */
	schedule_work(&ctx->put_work);

	return 0;
}