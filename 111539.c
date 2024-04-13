void tb_exec_unlock(TCGContext *tcg_ctx)
{
    tb_exec_change(tcg_ctx, false);
}