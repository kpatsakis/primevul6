RAMBlock *qemu_ram_alloc_from_fd(ram_addr_t size, MemoryRegion *mr,
                                 bool share, int fd,
                                 Error **errp)
{
    RAMBlock *new_block;
    Error *local_err = NULL;
    int64_t file_size;

    if (xen_enabled()) {
        error_setg(errp, "-mem-path not supported with Xen");
        return NULL;
    }

    if (kvm_enabled() && !kvm_has_sync_mmu()) {
        error_setg(errp,
                   "host lacks kvm mmu notifiers, -mem-path unsupported");
        return NULL;
    }

    if (phys_mem_alloc != qemu_anon_ram_alloc) {
        /*
         * file_ram_alloc() needs to allocate just like
         * phys_mem_alloc, but we haven't bothered to provide
         * a hook there.
         */
        error_setg(errp,
                   "-mem-path not supported with this accelerator");
        return NULL;
    }

    size = HOST_PAGE_ALIGN(size);
    file_size = get_file_size(fd);
    if (file_size > 0 && file_size < size) {
        error_setg(errp, "backing store %s size 0x%" PRIx64
                   " does not match 'size' option 0x" RAM_ADDR_FMT,
                   mem_path, file_size, size);
        return NULL;
    }

    new_block = g_malloc0(sizeof(*new_block));
    new_block->mr = mr;
    new_block->used_length = size;
    new_block->max_length = size;
    new_block->flags = share ? RAM_SHARED : 0;
    new_block->host = file_ram_alloc(new_block, size, fd, !file_size, errp);
    if (!new_block->host) {
        g_free(new_block);
        return NULL;
    }

    ram_block_add(new_block, &local_err);
    if (local_err) {
        g_free(new_block);
        error_propagate(errp, local_err);
        return NULL;
    }
    return new_block;

}