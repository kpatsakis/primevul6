static void fastrpc_notify_users(struct fastrpc_user *user)
{
	struct fastrpc_invoke_ctx *ctx;

	spin_lock(&user->lock);
	list_for_each_entry(ctx, &user->pending, node)
		complete(&ctx->work);
	spin_unlock(&user->lock);
}