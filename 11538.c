static int fastrpc_invoke_send(struct fastrpc_session_ctx *sctx,
			       struct fastrpc_invoke_ctx *ctx,
			       u32 kernel, uint32_t handle)
{
	struct fastrpc_channel_ctx *cctx;
	struct fastrpc_user *fl = ctx->fl;
	struct fastrpc_msg *msg = &ctx->msg;

	cctx = fl->cctx;
	msg->pid = fl->tgid;
	msg->tid = current->pid;

	if (kernel)
		msg->pid = 0;

	msg->ctx = ctx->ctxid | fl->pd;
	msg->handle = handle;
	msg->sc = ctx->sc;
	msg->addr = ctx->buf ? ctx->buf->phys : 0;
	msg->size = roundup(ctx->msg_sz, PAGE_SIZE);
	fastrpc_context_get(ctx);

	return rpmsg_send(cctx->rpdev->ept, (void *)msg, sizeof(*msg));
}