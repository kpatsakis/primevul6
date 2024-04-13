static void tb_phys_invalidate__locked(TCGContext *tcg_ctx, TranslationBlock *tb)
{
    do_tb_phys_invalidate(tcg_ctx, tb, true);
}