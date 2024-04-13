static inline void *alloc_code_gen_buffer(struct uc_struct *uc)
{
    TCGContext *tcg_ctx = uc->tcg_ctx;
    void *buf = static_code_gen_buffer;
    void *end = static_code_gen_buffer + sizeof(static_code_gen_buffer);
    size_t size;

    /* page-align the beginning and end of the buffer */
    buf = QEMU_ALIGN_PTR_UP(buf, uc->qemu_real_host_page_size);
    end = QEMU_ALIGN_PTR_DOWN(end, uc->qemu_real_host_page_size);

    size = end - buf;

    /* Honor a command-line option limiting the size of the buffer.  */
    if (size > tcg_ctx->code_gen_buffer_size) {
        size = QEMU_ALIGN_DOWN(tcg_ctx->code_gen_buffer_size,
                               uc->qemu_real_host_page_size);
    }
    tcg_ctx->code_gen_buffer_size = size;

#ifdef __mips__
    if (cross_256mb(buf, size)) {
        buf = split_cross_256mb(tcg_ctx, buf, size);
        size = tcg_ctx->code_gen_buffer_size;
    }
#endif

    if (qemu_mprotect_rwx(buf, size)) {
        abort();
    }
    qemu_madvise(buf, size, QEMU_MADV_HUGEPAGE);

    return buf;
}