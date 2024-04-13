static int fastrpc_put_args(struct fastrpc_invoke_ctx *ctx,
			    u32 kernel)
{
	struct fastrpc_remote_arg *rpra = ctx->rpra;
	int i, inbufs;

	inbufs = REMOTE_SCALARS_INBUFS(ctx->sc);

	for (i = inbufs; i < ctx->nbufs; ++i) {
		void *src = (void *)(uintptr_t)rpra[i].pv;
		void *dst = (void *)(uintptr_t)ctx->args[i].ptr;
		u64 len = rpra[i].len;

		if (!kernel) {
			if (copy_to_user((void __user *)dst, src, len))
				return -EFAULT;
		} else {
			memcpy(dst, src, len);
		}
	}

	return 0;
}