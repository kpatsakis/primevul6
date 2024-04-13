void address_space_init_dispatch(AddressSpace *as)
{
    as->dispatch = NULL;
    as->dispatch_listener = (MemoryListener) {
        .begin = mem_begin,
        .commit = mem_commit,
        .region_add = mem_add,
        .region_nop = mem_add,
        .priority = 0,
    };
    memory_listener_register(&as->dispatch_listener, as);
}