static void lo_map_remove(struct lo_map *map, size_t key)
{
    struct lo_map_elem *elem;

    if (key >= map->nelems) {
        return;
    }

    elem = &map->elems[key];
    if (!elem->in_use) {
        return;
    }

    elem->in_use = false;

    elem->freelist = map->freelist;
    map->freelist = key;
}