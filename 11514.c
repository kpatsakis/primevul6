static struct fastrpc_session_ctx *fastrpc_session_alloc(
					struct fastrpc_channel_ctx *cctx)
{
	struct fastrpc_session_ctx *session = NULL;
	unsigned long flags;
	int i;

	spin_lock_irqsave(&cctx->lock, flags);
	for (i = 0; i < cctx->sesscount; i++) {
		if (!cctx->session[i].used && cctx->session[i].valid) {
			cctx->session[i].used = true;
			session = &cctx->session[i];
			break;
		}
	}
	spin_unlock_irqrestore(&cctx->lock, flags);

	return session;
}