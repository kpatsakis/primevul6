static void *fastrpc_kmap(struct dma_buf *dmabuf, unsigned long pgnum)
{
	struct fastrpc_buf *buf = dmabuf->priv;

	return buf->virt ? buf->virt + pgnum * PAGE_SIZE : NULL;
}