static int fastrpc_internal_invoke(struct fastrpc_user *fl,  u32 kernel,
				   u32 handle, u32 sc,
				   struct fastrpc_invoke_args *args)
{
	struct fastrpc_invoke_ctx *ctx = NULL;
	int err = 0;

	if (!fl->sctx)
		return -EINVAL;

	if (!fl->cctx->rpdev)
		return -EPIPE;

	ctx = fastrpc_context_alloc(fl, kernel, sc, args);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	if (ctx->nscalars) {
		err = fastrpc_get_args(kernel, ctx);
		if (err)
			goto bail;
	}

	/* make sure that all CPU memory writes are seen by DSP */
	dma_wmb();
	/* Send invoke buffer to remote dsp */
	err = fastrpc_invoke_send(fl->sctx, ctx, kernel, handle);
	if (err)
		goto bail;

	/* Wait for remote dsp to respond or time out */
	err = wait_for_completion_interruptible(&ctx->work);
	if (err)
		goto bail;

	/* Check the response from remote dsp */
	err = ctx->retval;
	if (err)
		goto bail;

	if (ctx->nscalars) {
		/* make sure that all memory writes by DSP are seen by CPU */
		dma_rmb();
		/* populate all the output buffers with results */
		err = fastrpc_put_args(ctx, kernel);
		if (err)
			goto bail;
	}

bail:
	/* We are done with this compute context, remove it from pending list */
	spin_lock(&fl->lock);
	list_del(&ctx->node);
	spin_unlock(&fl->lock);
	fastrpc_context_put(ctx);

	if (err)
		dev_dbg(fl->sctx->dev, "Error: Invoke Failed %d\n", err);

	return err;
}