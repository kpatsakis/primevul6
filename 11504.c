static void fastrpc_rpmsg_remove(struct rpmsg_device *rpdev)
{
	struct fastrpc_channel_ctx *cctx = dev_get_drvdata(&rpdev->dev);
	struct fastrpc_user *user;
	unsigned long flags;

	spin_lock_irqsave(&cctx->lock, flags);
	list_for_each_entry(user, &cctx->users, user)
		fastrpc_notify_users(user);
	spin_unlock_irqrestore(&cctx->lock, flags);

	misc_deregister(&cctx->miscdev);
	of_platform_depopulate(&rpdev->dev);

	cctx->rpdev = NULL;
	fastrpc_channel_ctx_put(cctx);
}