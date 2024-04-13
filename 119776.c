static void lo_map_destroy(struct lo_map *map)
{
    free(map->elems);
}