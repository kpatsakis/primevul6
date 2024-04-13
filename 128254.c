address_space_extend_translation(AddressSpace *as, hwaddr addr, hwaddr target_len,
                                 MemoryRegion *mr, hwaddr base, hwaddr len,
                                 bool is_write)
{
    hwaddr done = 0;
    hwaddr xlat;
    MemoryRegion *this_mr;

    for (;;) {
        target_len -= len;
        addr += len;
        done += len;
        if (target_len == 0) {
            return done;
        }

        len = target_len;
        this_mr = address_space_translate(as, addr, &xlat, &len, is_write);
        if (this_mr != mr || xlat != base + done) {
            return done;
        }
    }
}