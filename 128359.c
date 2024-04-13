static bool notdirty_mem_accepts(void *opaque, hwaddr addr,
                                 unsigned size, bool is_write)
{
    return is_write;
}