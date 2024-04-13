static inline void *alloc_code_gen_buffer(struct uc_struct *uc)
{
    TCGContext *tcg_ctx = uc->tcg_ctx;
    int prot = PROT_WRITE | PROT_READ | PROT_EXEC;
    int flags = MAP_PRIVATE | MAP_ANONYMOUS;
    size_t size = tcg_ctx->code_gen_buffer_size;
    void *buf;
#ifdef USE_MAP_JIT
    flags |= MAP_JIT;
#endif
    buf = mmap(NULL, size, prot, flags, -1, 0);
    if (buf == MAP_FAILED) {
        return NULL;
    }

#ifdef __mips__
    if (cross_256mb(buf, size)) {
        /*
         * Try again, with the original still mapped, to avoid re-acquiring
         * the same 256mb crossing.
         */
        size_t size2;
        void *buf2 = mmap(NULL, size, prot, flags, -1, 0);
        switch ((int)(buf2 != MAP_FAILED)) {
        case 1:
            if (!cross_256mb(buf2, size)) {
                /* Success!  Use the new buffer.  */
                munmap(buf, size);
                break;
            }
            /* Failure.  Work with what we had.  */
            munmap(buf2, size);
            /* fallthru */
        default:
            /* Split the original buffer.  Free the smaller half.  */
            buf2 = split_cross_256mb(tcg_ctx, buf, size);
            size2 = tcg_ctx->code_gen_buffer_size;
            if (buf == buf2) {
                munmap(buf + size2, size - size2);
            } else {
                munmap(buf, size - size2);
            }
            size = size2;
            break;
        }
        buf = buf2;
    }
#endif

    /* Request large pages for the buffer.  */
    qemu_madvise(buf, size, QEMU_MADV_HUGEPAGE);

    return buf;
}