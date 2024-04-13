int ram_block_discard_range(RAMBlock *rb, uint64_t start, size_t length)
{
    int ret = -1;

    uint8_t *host_startaddr = rb->host + start;

    if ((uintptr_t)host_startaddr & (rb->page_size - 1)) {
        error_report("ram_block_discard_range: Unaligned start address: %p",
                     host_startaddr);
        goto err;
    }

    if ((start + length) <= rb->used_length) {
        uint8_t *host_endaddr = host_startaddr + length;
        if ((uintptr_t)host_endaddr & (rb->page_size - 1)) {
            error_report("ram_block_discard_range: Unaligned end address: %p",
                         host_endaddr);
            goto err;
        }

        errno = ENOTSUP; /* If we are missing MADVISE etc */

        if (rb->page_size == qemu_host_page_size) {
#if defined(CONFIG_MADVISE)
            /* Note: We need the madvise MADV_DONTNEED behaviour of definitely
             * freeing the page.
             */
            ret = madvise(host_startaddr, length, MADV_DONTNEED);
#endif
        } else {
            /* Huge page case  - unfortunately it can't do DONTNEED, but
             * it can do the equivalent by FALLOC_FL_PUNCH_HOLE in the
             * huge page file.
             */
#ifdef CONFIG_FALLOCATE_PUNCH_HOLE
            ret = fallocate(rb->fd, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE,
                            start, length);
#endif
        }
        if (ret) {
            ret = -errno;
            error_report("ram_block_discard_range: Failed to discard range "
                         "%s:%" PRIx64 " +%zx (%d)",
                         rb->idstr, start, length, ret);
        }
    } else {
        error_report("ram_block_discard_range: Overrun block '%s' (%" PRIu64
                     "/%zx/" RAM_ADDR_FMT")",
                     rb->idstr, start, length, rb->used_length);
    }

err:
    return ret;
}