static inline void page_lock_tb(struct uc_struct *uc, const TranslationBlock *tb)
{
    page_lock_pair(uc, NULL, tb->page_addr[0], NULL, tb->page_addr[1], 0);
}