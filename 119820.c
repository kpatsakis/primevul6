static void lo_map_init(struct lo_map *map)
{
    map->elems = NULL;
    map->nelems = 0;
    map->freelist = -1;
}