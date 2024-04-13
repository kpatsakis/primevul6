void tb_exec_lock(TCGContext *tcg_ctx)
{
    /* assumes sys_icache_invalidate already called */
    tb_exec_change(tcg_ctx, true);
}