static void fastrpc_buf_free(struct fastrpc_buf *buf)
{
	dma_free_coherent(buf->dev, buf->size, buf->virt,
			  FASTRPC_PHYS(buf->phys));
	kfree(buf);
}