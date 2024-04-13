static int memory_try_enable_merging(void *addr, size_t len)
{
    if (!machine_mem_merge(current_machine)) {
        /* disabled by the user */
        return 0;
    }

    return qemu_madvise(addr, len, QEMU_MADV_MERGEABLE);
}