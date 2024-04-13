static inline void tb_page_remove(PageDesc *pd, TranslationBlock *tb)
{
    TranslationBlock *tb1;
    uintptr_t *pprev;
    unsigned int n1;

    assert_page_locked(pd);
    pprev = &pd->first_tb;
    PAGE_FOR_EACH_TB(pd, tb1, n1) {
        if (tb1 == tb) {
            *pprev = tb1->page_next[n1];
            return;
        }
        pprev = &tb1->page_next[n1];
    }
    g_assert_not_reached();
}