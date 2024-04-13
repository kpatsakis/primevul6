static struct lo_map_elem *lo_map_get(struct lo_map *map, size_t key)
{
    if (key >= map->nelems) {
        return NULL;
    }
    if (!map->elems[key].in_use) {
        return NULL;
    }
    return &map->elems[key];
}