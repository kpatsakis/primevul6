static int fastrpc_device_open(struct inode *inode, struct file *filp)
{
	struct fastrpc_channel_ctx *cctx = miscdev_to_cctx(filp->private_data);
	struct fastrpc_user *fl = NULL;
	unsigned long flags;

	fl = kzalloc(sizeof(*fl), GFP_KERNEL);
	if (!fl)
		return -ENOMEM;

	/* Released in fastrpc_device_release() */
	fastrpc_channel_ctx_get(cctx);

	filp->private_data = fl;
	spin_lock_init(&fl->lock);
	mutex_init(&fl->mutex);
	INIT_LIST_HEAD(&fl->pending);
	INIT_LIST_HEAD(&fl->maps);
	INIT_LIST_HEAD(&fl->user);
	fl->tgid = current->tgid;
	fl->cctx = cctx;

	fl->sctx = fastrpc_session_alloc(cctx);
	if (!fl->sctx) {
		dev_err(&cctx->rpdev->dev, "No session available\n");
		mutex_destroy(&fl->mutex);
		kfree(fl);

		return -EBUSY;
	}

	spin_lock_irqsave(&cctx->lock, flags);
	list_add_tail(&fl->user, &cctx->users);
	spin_unlock_irqrestore(&cctx->lock, flags);

	return 0;
}