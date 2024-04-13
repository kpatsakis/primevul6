static void *fastrpc_vmap(struct dma_buf *dmabuf)
{
	struct fastrpc_buf *buf = dmabuf->priv;

	return buf->virt;
}