static int fastrpc_map_find(struct fastrpc_user *fl, int fd,
			    struct fastrpc_map **ppmap)
{
	struct fastrpc_map *map = NULL;

	mutex_lock(&fl->mutex);
	list_for_each_entry(map, &fl->maps, node) {
		if (map->fd == fd) {
			fastrpc_map_get(map);
			*ppmap = map;
			mutex_unlock(&fl->mutex);
			return 0;
		}
	}
	mutex_unlock(&fl->mutex);

	return -ENOENT;
}