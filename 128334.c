static bool subpage_accepts(void *opaque, hwaddr addr,
                            unsigned len, bool is_write)
{
    subpage_t *subpage = opaque;
#if defined(DEBUG_SUBPAGE)
    printf("%s: subpage %p %c len %u addr " TARGET_FMT_plx "\n",
           __func__, subpage, is_write ? 'w' : 'r', len, addr);
#endif

    return address_space_access_valid(subpage->as, addr + subpage->base,
                                      len, is_write);
}