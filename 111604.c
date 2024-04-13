tb_link_page(struct uc_struct *uc, TranslationBlock *tb, tb_page_addr_t phys_pc,
             tb_page_addr_t phys_page2)
{
    PageDesc *p;
    PageDesc *p2 = NULL;

    assert_memory_lock();

    if (phys_pc == -1) {
        /*
         * If the TB is not associated with a physical RAM page then
         * it must be a temporary one-insn TB, and we have nothing to do
         * except fill in the page_addr[] fields.
         */
        assert(tb->cflags & CF_NOCACHE);
        tb->page_addr[0] = tb->page_addr[1] = -1;
        return tb;
    }

    /*
     * Add the TB to the page list, acquiring first the pages's locks.
     * We keep the locks held until after inserting the TB in the hash table,
     * so that if the insertion fails we know for sure that the TBs are still
     * in the page descriptors.
     * Note that inserting into the hash table first isn't an option, since
     * we can only insert TBs that are fully initialized.
     */
    page_lock_pair(uc, &p, phys_pc, &p2, phys_page2, 1);
    tb_page_add(uc, p, tb, 0, phys_pc & TARGET_PAGE_MASK);
    if (p2) {
        tb_page_add(uc, p2, tb, 1, phys_page2);
    } else {
        tb->page_addr[1] = -1;
    }

    if (!(tb->cflags & CF_NOCACHE)) {
        void *existing_tb = NULL;
        uint32_t h;

        /* add in the hash table */
        h = tb_hash_func(phys_pc, tb->pc, tb->flags, tb->cflags & CF_HASH_MASK,
                         tb->trace_vcpu_dstate);
        tb->hash = h;   // unicorn needs this so it can remove this tb
        qht_insert(uc, &uc->tcg_ctx->tb_ctx.htable, tb, h, &existing_tb);

        /* remove TB from the page(s) if we couldn't insert it */
        if (unlikely(existing_tb)) {
            tb_page_remove(p, tb);
            invalidate_page_bitmap(p);
            if (p2) {
                tb_page_remove(p2, tb);
                invalidate_page_bitmap(p2);
            }
            tb = existing_tb;
        }
    }

    if (p2 && p2 != p) {
        page_unlock(p2);
    }
    page_unlock(p);

    return tb;
}