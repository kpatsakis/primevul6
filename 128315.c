static uint32_t phys_map_node_alloc(PhysPageMap *map, bool leaf)
{
    unsigned i;
    uint32_t ret;
    PhysPageEntry e;
    PhysPageEntry *p;

    ret = map->nodes_nb++;
    p = map->nodes[ret];
    assert(ret != PHYS_MAP_NODE_NIL);
    assert(ret != map->nodes_nb_alloc);

    e.skip = leaf ? 0 : 1;
    e.ptr = leaf ? PHYS_SECTION_UNASSIGNED : PHYS_MAP_NODE_NIL;
    for (i = 0; i < P_L2_SIZE; ++i) {
        memcpy(&p[i], &e, sizeof(e));
    }
    return ret;
}