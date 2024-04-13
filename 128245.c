MemTxResult address_space_rw(AddressSpace *as, hwaddr addr, MemTxAttrs attrs,
                             uint8_t *buf, int len, bool is_write)
{
    if (is_write) {
        return address_space_write(as, addr, attrs, (uint8_t *)buf, len);
    } else {
        return address_space_read(as, addr, attrs, (uint8_t *)buf, len);
    }
}