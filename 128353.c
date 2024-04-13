static void phys_map_node_reserve(PhysPageMap *map, unsigned nodes)
{
    static unsigned alloc_hint = 16;
    if (map->nodes_nb + nodes > map->nodes_nb_alloc) {
        map->nodes_nb_alloc = MAX(map->nodes_nb_alloc, alloc_hint);
        map->nodes_nb_alloc = MAX(map->nodes_nb_alloc, map->nodes_nb + nodes);
        map->nodes = g_renew(Node, map->nodes, map->nodes_nb_alloc);
        alloc_hint = map->nodes_nb_alloc;
    }
}