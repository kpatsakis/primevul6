static void fastrpc_release(struct dma_buf *dmabuf)
{
	struct fastrpc_buf *buffer = dmabuf->priv;

	fastrpc_buf_free(buffer);
}