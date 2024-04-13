static void tb_exec_change(TCGContext *tcg_ctx, bool locked)
{
    jit_write_protect(locked);
    tcg_ctx->code_gen_locked = locked;
}