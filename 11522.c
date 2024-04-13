static void fastrpc_unmap_dma_buf(struct dma_buf_attachment *attach,
				  struct sg_table *table,
				  enum dma_data_direction dir)
{
	dma_unmap_sg(attach->dev, table->sgl, table->nents, dir);
}