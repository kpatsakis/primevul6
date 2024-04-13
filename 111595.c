static inline void page_unlock_tb(struct uc_struct *uc, const TranslationBlock *tb)
{
    PageDesc *p1 = page_find(uc, tb->page_addr[0] >> TARGET_PAGE_BITS);

    page_unlock(p1);
    if (unlikely(tb->page_addr[1] != -1)) {
        PageDesc *p2 = page_find(uc, tb->page_addr[1] >> TARGET_PAGE_BITS);

        if (p2 != p1) {
            page_unlock(p2);
        }
    }
}