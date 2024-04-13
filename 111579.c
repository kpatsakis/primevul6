static bool tb_exec_is_locked(TCGContext *tcg_ctx)
{
    return tcg_ctx->code_gen_locked;
}