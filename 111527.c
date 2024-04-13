static inline void *alloc_code_gen_buffer(struct uc_struct *uc)
{
    TCGContext *tcg_ctx = uc->tcg_ctx;
    size_t size = tcg_ctx->code_gen_buffer_size;
    return VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT,
                        PAGE_EXECUTE_READWRITE);
}