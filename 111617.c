static void do_tb_phys_invalidate(TCGContext *tcg_ctx, TranslationBlock *tb, bool rm_from_page_list)
{
    CPUState *cpu = tcg_ctx->uc->cpu;
    struct uc_struct *uc = tcg_ctx->uc;
    PageDesc *p;
    uint32_t h;
    tb_page_addr_t phys_pc;
    bool code_gen_locked;

    assert_memory_lock();
    code_gen_locked = tb_exec_is_locked(tcg_ctx);
    tb_exec_unlock(tcg_ctx);

    /* make sure no further incoming jumps will be chained to this TB */
    tb->cflags = tb->cflags | CF_INVALID;

    /* remove the TB from the hash list */
    phys_pc = tb->page_addr[0] + (tb->pc & ~TARGET_PAGE_MASK);
    h = tb_hash_func(phys_pc, tb->pc, tb->flags, tb_cflags(tb) & CF_HASH_MASK,
                     tb->trace_vcpu_dstate);
    if (!(tb->cflags & CF_NOCACHE) &&
        !qht_remove(&tcg_ctx->tb_ctx.htable, tb, h)) {
        tb_exec_change(tcg_ctx, code_gen_locked);
        return;
    }

    /* remove the TB from the page list */
    if (rm_from_page_list) {
        p = page_find(tcg_ctx->uc, tb->page_addr[0] >> TARGET_PAGE_BITS);
        tb_page_remove(p, tb);
        invalidate_page_bitmap(p);
        if (tb->page_addr[1] != -1) {
            p = page_find(tcg_ctx->uc, tb->page_addr[1] >> TARGET_PAGE_BITS);
            tb_page_remove(p, tb);
            invalidate_page_bitmap(p);
        }
    }

    /* remove the TB from the hash list */
    h = tb_jmp_cache_hash_func(uc, tb->pc);
    if (cpu->tb_jmp_cache[h] == tb) {
        cpu->tb_jmp_cache[h] = NULL;
    }

    /* suppress this TB from the two jump lists */
    tb_remove_from_jmp_list(tb, 0);
    tb_remove_from_jmp_list(tb, 1);

    /* suppress any remaining jumps to this TB */
    tb_jmp_unlink(tb);

    tcg_ctx->tb_phys_invalidate_count = tcg_ctx->tb_phys_invalidate_count + 1;

    tb_exec_change(tcg_ctx, code_gen_locked);
}