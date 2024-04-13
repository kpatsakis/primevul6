void free_code_gen_buffer(struct uc_struct *uc)
{
    TCGContext *tcg_ctx = uc->tcg_ctx;
    if (tcg_ctx->code_gen_buffer) {
        VirtualFree(tcg_ctx->code_gen_buffer, 0, MEM_RELEASE);
    }
}