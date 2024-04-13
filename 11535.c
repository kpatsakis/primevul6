fastrpc_map_dma_buf(struct dma_buf_attachment *attachment,
		    enum dma_data_direction dir)
{
	struct fastrpc_dma_buf_attachment *a = attachment->priv;
	struct sg_table *table;

	table = &a->sgt;

	if (!dma_map_sg(attachment->dev, table->sgl, table->nents, dir))
		return ERR_PTR(-ENOMEM);

	return table;
}