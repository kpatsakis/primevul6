void *address_space_map(AddressSpace *as,
                        hwaddr addr,
                        hwaddr *plen,
                        bool is_write)
{
    hwaddr len = *plen;
    hwaddr l, xlat;
    MemoryRegion *mr;
    void *ptr;

    if (len == 0) {
        return NULL;
    }

    l = len;
    rcu_read_lock();
    mr = address_space_translate(as, addr, &xlat, &l, is_write);

    if (!memory_access_is_direct(mr, is_write)) {
        if (atomic_xchg(&bounce.in_use, true)) {
            rcu_read_unlock();
            return NULL;
        }
        /* Avoid unbounded allocations */
        l = MIN(l, TARGET_PAGE_SIZE);
        bounce.buffer = qemu_memalign(TARGET_PAGE_SIZE, l);
        bounce.addr = addr;
        bounce.len = l;

        memory_region_ref(mr);
        bounce.mr = mr;
        if (!is_write) {
            address_space_read(as, addr, MEMTXATTRS_UNSPECIFIED,
                               bounce.buffer, l);
        }

        rcu_read_unlock();
        *plen = l;
        return bounce.buffer;
    }


    memory_region_ref(mr);
    *plen = address_space_extend_translation(as, addr, len, mr, xlat, l, is_write);
    ptr = qemu_ram_ptr_length(mr->ram_block, xlat, plen);
    rcu_read_unlock();

    return ptr;
}