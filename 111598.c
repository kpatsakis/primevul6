void free_code_gen_buffer(struct uc_struct *uc)
{
    TCGContext *tcg_ctx = uc->tcg_ctx;
    if (tcg_ctx->code_gen_buffer) {
        munmap(tcg_ctx->code_gen_buffer, tcg_ctx->code_gen_buffer_size);
    }
}