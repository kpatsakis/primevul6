static inline void tb_page_add(struct uc_struct *uc, PageDesc *p, TranslationBlock *tb,
                               unsigned int n, tb_page_addr_t page_addr)
{
    bool page_already_protected;

    assert_page_locked(p);

    tb->page_addr[n] = page_addr;
    tb->page_next[n] = p->first_tb;
    page_already_protected = p->first_tb != (uintptr_t)NULL;
    p->first_tb = (uintptr_t)tb | n;
    invalidate_page_bitmap(p);

    /* if some code is already present, then the pages are already
       protected. So we handle the case where only the first TB is
       allocated in a physical page */
    if (!page_already_protected) {
        tlb_protect_code(uc, page_addr);
    }
}