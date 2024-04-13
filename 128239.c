static void *file_ram_alloc(RAMBlock *block,
                            ram_addr_t memory,
                            int fd,
                            bool truncate,
                            Error **errp)
{
    void *area;

    block->page_size = qemu_fd_getpagesize(fd);
    block->mr->align = block->page_size;
#if defined(__s390x__)
    if (kvm_enabled()) {
        block->mr->align = MAX(block->mr->align, QEMU_VMALLOC_ALIGN);
    }
#endif

    if (memory < block->page_size) {
        error_setg(errp, "memory size 0x" RAM_ADDR_FMT " must be equal to "
                   "or larger than page size 0x%zx",
                   memory, block->page_size);
        return NULL;
    }

    memory = ROUND_UP(memory, block->page_size);

    /*
     * ftruncate is not supported by hugetlbfs in older
     * hosts, so don't bother bailing out on errors.
     * If anything goes wrong with it under other filesystems,
     * mmap will fail.
     *
     * Do not truncate the non-empty backend file to avoid corrupting
     * the existing data in the file. Disabling shrinking is not
     * enough. For example, the current vNVDIMM implementation stores
     * the guest NVDIMM labels at the end of the backend file. If the
     * backend file is later extended, QEMU will not be able to find
     * those labels. Therefore, extending the non-empty backend file
     * is disabled as well.
     */
    if (truncate && ftruncate(fd, memory)) {
        perror("ftruncate");
    }

    area = qemu_ram_mmap(fd, memory, block->mr->align,
                         block->flags & RAM_SHARED);
    if (area == MAP_FAILED) {
        error_setg_errno(errp, errno,
                         "unable to map backing store for guest RAM");
        return NULL;
    }

    if (mem_prealloc) {
        os_mem_prealloc(fd, area, memory, smp_cpus, errp);
        if (errp && *errp) {
            qemu_ram_munmap(area, memory);
            return NULL;
        }
    }

    block->fd = fd;
    return area;
}