RAMBlock *qemu_ram_alloc_from_file(ram_addr_t size, MemoryRegion *mr,
                                   bool share, const char *mem_path,
                                   Error **errp)
{
    int fd;
    bool created;
    RAMBlock *block;

    fd = file_ram_open(mem_path, memory_region_name(mr), &created, errp);
    if (fd < 0) {
        return NULL;
    }

    block = qemu_ram_alloc_from_fd(size, mr, share, fd, errp);
    if (!block) {
        if (created) {
            unlink(mem_path);
        }
        close(fd);
        return NULL;
    }

    return block;
}