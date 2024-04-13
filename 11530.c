static int fastrpc_device_release(struct inode *inode, struct file *file)
{
	struct fastrpc_user *fl = (struct fastrpc_user *)file->private_data;
	struct fastrpc_channel_ctx *cctx = fl->cctx;
	struct fastrpc_invoke_ctx *ctx, *n;
	struct fastrpc_map *map, *m;
	unsigned long flags;

	fastrpc_release_current_dsp_process(fl);

	spin_lock_irqsave(&cctx->lock, flags);
	list_del(&fl->user);
	spin_unlock_irqrestore(&cctx->lock, flags);

	if (fl->init_mem)
		fastrpc_buf_free(fl->init_mem);

	list_for_each_entry_safe(ctx, n, &fl->pending, node) {
		list_del(&ctx->node);
		fastrpc_context_put(ctx);
	}

	list_for_each_entry_safe(map, m, &fl->maps, node) {
		list_del(&map->node);
		fastrpc_map_put(map);
	}

	fastrpc_session_free(cctx, fl->sctx);
	fastrpc_channel_ctx_put(cctx);

	mutex_destroy(&fl->mutex);
	kfree(fl);
	file->private_data = NULL;

	return 0;
}