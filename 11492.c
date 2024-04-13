static void fastrpc_context_put_wq(struct work_struct *work)
{
	struct fastrpc_invoke_ctx *ctx =
			container_of(work, struct fastrpc_invoke_ctx, put_work);

	fastrpc_context_put(ctx);
}