static u64 fastrpc_get_payload_size(struct fastrpc_invoke_ctx *ctx, int metalen)
{
	u64 size = 0;
	int i;

	size = ALIGN(metalen, FASTRPC_ALIGN);
	for (i = 0; i < ctx->nscalars; i++) {
		if (ctx->args[i].fd == 0 || ctx->args[i].fd == -1) {

			if (ctx->olaps[i].offset == 0)
				size = ALIGN(size, FASTRPC_ALIGN);

			size += (ctx->olaps[i].mend - ctx->olaps[i].mstart);
		}
	}

	return size;
}