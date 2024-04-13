static struct lo_map_elem *lo_map_reserve(struct lo_map *map, size_t key)
{
    ssize_t *prev;

    if (!lo_map_grow(map, key + 1)) {
        return NULL;
    }

    for (prev = &map->freelist; *prev != -1;
         prev = &map->elems[*prev].freelist) {
        if (*prev == key) {
            struct lo_map_elem *elem = &map->elems[key];

            *prev = elem->freelist;
            elem->in_use = true;
            return elem;
        }
    }
    return NULL;
}