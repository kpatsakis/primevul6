MemTxResult address_space_read_continue(AddressSpace *as, hwaddr addr,
                                        MemTxAttrs attrs, uint8_t *buf,
                                        int len, hwaddr addr1, hwaddr l,
                                        MemoryRegion *mr)
{
    uint8_t *ptr;
    uint64_t val;
    MemTxResult result = MEMTX_OK;
    bool release_lock = false;

    for (;;) {
        if (!memory_access_is_direct(mr, false)) {
            /* I/O case */
            release_lock |= prepare_mmio_access(mr);
            l = memory_access_size(mr, l, addr1);
            switch (l) {
            case 8:
                /* 64 bit read access */
                result |= memory_region_dispatch_read(mr, addr1, &val, 8,
                                                      attrs);
                stq_p(buf, val);
                break;
            case 4:
                /* 32 bit read access */
                result |= memory_region_dispatch_read(mr, addr1, &val, 4,
                                                      attrs);
                stl_p(buf, val);
                break;
            case 2:
                /* 16 bit read access */
                result |= memory_region_dispatch_read(mr, addr1, &val, 2,
                                                      attrs);
                stw_p(buf, val);
                break;
            case 1:
                /* 8 bit read access */
                result |= memory_region_dispatch_read(mr, addr1, &val, 1,
                                                      attrs);
                stb_p(buf, val);
                break;
            default:
                abort();
            }
        } else {
            /* RAM case */
            ptr = qemu_ram_ptr_length(mr->ram_block, addr1, &l);
            memcpy(buf, ptr, l);
        }

        if (release_lock) {
            qemu_mutex_unlock_iothread();
            release_lock = false;
        }

        len -= l;
        buf += l;
        addr += l;

        if (!len) {
            break;
        }

        l = len;
        mr = address_space_translate(as, addr, &addr1, &l, false);
    }

    return result;
}