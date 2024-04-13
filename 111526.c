void tb_phys_invalidate(TCGContext *tcg_ctx, TranslationBlock *tb, tb_page_addr_t page_addr)
{
    if (page_addr == -1 && tb->page_addr[0] != -1) {
        page_lock_tb(tcg_ctx->uc, tb);
        do_tb_phys_invalidate(tcg_ctx, tb, true);
        page_unlock_tb(tcg_ctx->uc, tb);
    } else {
        do_tb_phys_invalidate(tcg_ctx, tb, false);
    }
}