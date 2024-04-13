static inline void *split_cross_256mb(TCGContext *tcg_ctx, void *buf1, size_t size1)
{
    void *buf2 = (void *)(((uintptr_t)buf1 + size1) & ~0x0ffffffful);
    size_t size2 = buf1 + size1 - buf2;

    size1 = buf2 - buf1;
    if (size1 < size2) {
        size1 = size2;
        buf1 = buf2;
    }

    tcg_ctx->code_gen_buffer_size = size1;
    return buf1;
}