static void fastrpc_map_get(struct fastrpc_map *map)
{
	if (map)
		kref_get(&map->refcount);
}