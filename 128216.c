void address_space_destroy_dispatch(AddressSpace *as)
{
    AddressSpaceDispatch *d = as->dispatch;

    atomic_rcu_set(&as->dispatch, NULL);
    if (d) {
        call_rcu(d, address_space_dispatch_free, rcu);
    }
}