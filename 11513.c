static void fastrpc_map_put(struct fastrpc_map *map)
{
	if (map)
		kref_put(&map->refcount, fastrpc_free_map);
}