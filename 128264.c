static void phys_page_compact_all(AddressSpaceDispatch *d, int nodes_nb)
{
    if (d->phys_map.skip) {
        phys_page_compact(&d->phys_map, d->map.nodes);
    }
}