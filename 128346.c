void cpu_flush_icache_range(hwaddr start, int len)
{
    /*
     * This function should do the same thing as an icache flush that was
     * triggered from within the guest. For TCG we are always cache coherent,
     * so there is no need to flush anything. For KVM / Xen we need to flush
     * the host's instruction cache at least.
     */
    if (tcg_enabled()) {
        return;
    }

    cpu_physical_memory_write_rom_internal(&address_space_memory,
                                           start, NULL, len, FLUSH_CACHE);
}