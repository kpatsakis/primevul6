static MemTxResult subpage_read(void *opaque, hwaddr addr, uint64_t *data,
                                unsigned len, MemTxAttrs attrs)
{
    subpage_t *subpage = opaque;
    uint8_t buf[8];
    MemTxResult res;

#if defined(DEBUG_SUBPAGE)
    printf("%s: subpage %p len %u addr " TARGET_FMT_plx "\n", __func__,
           subpage, len, addr);
#endif
    res = address_space_read(subpage->as, addr + subpage->base,
                             attrs, buf, len);
    if (res) {
        return res;
    }
    switch (len) {
    case 1:
        *data = ldub_p(buf);
        return MEMTX_OK;
    case 2:
        *data = lduw_p(buf);
        return MEMTX_OK;
    case 4:
        *data = ldl_p(buf);
        return MEMTX_OK;
    case 8:
        *data = ldq_p(buf);
        return MEMTX_OK;
    default:
        abort();
    }
}