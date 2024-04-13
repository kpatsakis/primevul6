static int fastrpc_get_meta_size(struct fastrpc_invoke_ctx *ctx)
{
	int size = 0;

	size = (sizeof(struct fastrpc_remote_arg) +
		sizeof(struct fastrpc_invoke_buf) +
		sizeof(struct fastrpc_phy_page)) * ctx->nscalars +
		sizeof(u64) * FASTRPC_MAX_FDLIST +
		sizeof(u32) * FASTRPC_MAX_CRCLIST;

	return size;
}