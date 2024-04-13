static MemTxResult subpage_write(void *opaque, hwaddr addr,
                                 uint64_t value, unsigned len, MemTxAttrs attrs)
{
    subpage_t *subpage = opaque;
    uint8_t buf[8];

#if defined(DEBUG_SUBPAGE)
    printf("%s: subpage %p len %u addr " TARGET_FMT_plx
           " value %"PRIx64"\n",
           __func__, subpage, len, addr, value);
#endif
    switch (len) {
    case 1:
        stb_p(buf, value);
        break;
    case 2:
        stw_p(buf, value);
        break;
    case 4:
        stl_p(buf, value);
        break;
    case 8:
        stq_p(buf, value);
        break;
    default:
        abort();
    }
    return address_space_write(subpage->as, addr + subpage->base,
                               attrs, buf, len);
}