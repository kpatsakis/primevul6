static struct lo_map_elem *lo_map_alloc_elem(struct lo_map *map)
{
    struct lo_map_elem *elem;

    if (map->freelist == -1 && !lo_map_grow(map, map->nelems + 256)) {
        return NULL;
    }

    elem = &map->elems[map->freelist];
    map->freelist = elem->freelist;

    elem->in_use = true;

    return elem;
}