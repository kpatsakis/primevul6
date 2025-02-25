static MemTxResult address_space_write_continue(AddressSpace *as, hwaddr addr,
                                                MemTxAttrs attrs,
                                                const uint8_t *buf,
                                                int len, hwaddr addr1,
                                                hwaddr l, MemoryRegion *mr)
{
    uint8_t *ptr;
    uint64_t val;
    MemTxResult result = MEMTX_OK;
    bool release_lock = false;

    for (;;) {
        if (!memory_access_is_direct(mr, true)) {
            release_lock |= prepare_mmio_access(mr);
            l = memory_access_size(mr, l, addr1);
            /* XXX: could force current_cpu to NULL to avoid
               potential bugs */
            switch (l) {
            case 8:
                /* 64 bit write access */
                val = ldq_p(buf);
                result |= memory_region_dispatch_write(mr, addr1, val, 8,
                                                       attrs);
                break;
            case 4:
                /* 32 bit write access */
                val = (uint32_t)ldl_p(buf);
                result |= memory_region_dispatch_write(mr, addr1, val, 4,
                                                       attrs);
                break;
            case 2:
                /* 16 bit write access */
                val = lduw_p(buf);
                result |= memory_region_dispatch_write(mr, addr1, val, 2,
                                                       attrs);
                break;
            case 1:
                /* 8 bit write access */
                val = ldub_p(buf);
                result |= memory_region_dispatch_write(mr, addr1, val, 1,
                                                       attrs);
                break;
            default:
                abort();
            }
        } else {
            /* RAM case */
            ptr = qemu_ram_ptr_length(mr->ram_block, addr1, &l);
            memcpy(ptr, buf, l);
            invalidate_and_set_dirty(mr, addr1, l);
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
        mr = address_space_translate(as, addr, &addr1, &l, true);
    }

    return result;
}