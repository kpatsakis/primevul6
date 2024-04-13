MemTxResult address_space_read_full(AddressSpace *as, hwaddr addr,
                                    MemTxAttrs attrs, uint8_t *buf, int len)
{
    hwaddr l;
    hwaddr addr1;
    MemoryRegion *mr;
    MemTxResult result = MEMTX_OK;

    if (len > 0) {
        rcu_read_lock();
        l = len;
        mr = address_space_translate(as, addr, &addr1, &l, false);
        result = address_space_read_continue(as, addr, attrs, buf, len,
                                             addr1, l, mr);
        rcu_read_unlock();
    }

    return result;
}