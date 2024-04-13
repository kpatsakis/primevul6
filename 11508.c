static void fastrpc_free_map(struct kref *ref)
{
	struct fastrpc_map *map;

	map = container_of(ref, struct fastrpc_map, refcount);

	if (map->table) {
		dma_buf_unmap_attachment(map->attach, map->table,
					 DMA_BIDIRECTIONAL);
		dma_buf_detach(map->buf, map->attach);
		dma_buf_put(map->buf);
	}

	kfree(map);
}