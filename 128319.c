static void mem_commit(MemoryListener *listener)
{
    AddressSpace *as = container_of(listener, AddressSpace, dispatch_listener);
    AddressSpaceDispatch *cur = as->dispatch;
    AddressSpaceDispatch *next = as->next_dispatch;

    phys_page_compact_all(next, next->map.nodes_nb);

    atomic_rcu_set(&as->dispatch, next);
    if (cur) {
        call_rcu(cur, address_space_dispatch_free, rcu);
    }
}