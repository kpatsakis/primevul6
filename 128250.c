long qemu_getrampagesize(void)
{
    long hpsize = LONG_MAX;
    long mainrampagesize;
    Object *memdev_root;

    if (mem_path) {
        mainrampagesize = qemu_mempath_getpagesize(mem_path);
    } else {
        mainrampagesize = getpagesize();
    }

    /* it's possible we have memory-backend objects with
     * hugepage-backed RAM. these may get mapped into system
     * address space via -numa parameters or memory hotplug
     * hooks. we want to take these into account, but we
     * also want to make sure these supported hugepage
     * sizes are applicable across the entire range of memory
     * we may boot from, so we take the min across all
     * backends, and assume normal pages in cases where a
     * backend isn't backed by hugepages.
     */
    memdev_root = object_resolve_path("/objects", NULL);
    if (memdev_root) {
        object_child_foreach(memdev_root, find_max_supported_pagesize, &hpsize);
    }
    if (hpsize == LONG_MAX) {
        /* No additional memory regions found ==> Report main RAM page size */
        return mainrampagesize;
    }

    /* If NUMA is disabled or the NUMA nodes are not backed with a
     * memory-backend, then there is at least one node using "normal" RAM,
     * so if its page size is smaller we have got to report that size instead.
     */
    if (hpsize > mainrampagesize &&
        (nb_numa_nodes == 0 || numa_info[0].node_memdev == NULL)) {
        static bool warned;
        if (!warned) {
            error_report("Huge page support disabled (n/a for main memory).");
            warned = true;
        }
        return mainrampagesize;
    }

    return hpsize;
}