int64_t address_space_cache_init(MemoryRegionCache *cache,
                                 AddressSpace *as,
                                 hwaddr addr,
                                 hwaddr len,
                                 bool is_write)
{
    cache->len = len;
    cache->as = as;
    cache->xlat = addr;
    return len;
}