static inline void code_gen_alloc(struct uc_struct *uc, size_t tb_size)
{
    TCGContext *tcg_ctx = uc->tcg_ctx;
    tcg_ctx->code_gen_buffer_size = size_code_gen_buffer(tb_size);
    tcg_ctx->code_gen_buffer = alloc_code_gen_buffer(uc);
    if (tcg_ctx->code_gen_buffer == NULL) {
        fprintf(stderr, "Could not allocate dynamic translator buffer\n");
        exit(1);
    }
}