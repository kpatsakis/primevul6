page_collection_lock(struct uc_struct *uc, tb_page_addr_t start, tb_page_addr_t end)
{
    struct page_collection *set = g_malloc(sizeof(*set));
    tb_page_addr_t index;
    PageDesc *pd;

    start >>= TARGET_PAGE_BITS;
    end   >>= TARGET_PAGE_BITS;
    g_assert(start <= end);

    set->tree = g_tree_new_full(tb_page_addr_cmp, NULL, NULL,
                                page_entry_destroy);
    set->max = NULL;
    assert_no_pages_locked();

 retry:
#if 0
    g_tree_foreach(set->tree, page_entry_lock, NULL);
#endif

    for (index = start; index <= end; index++) {
        TranslationBlock *tb;
        int n;

        pd = page_find(uc, index);
        if (pd == NULL) {
            continue;
        }
        if (page_trylock_add(uc, set, index << TARGET_PAGE_BITS)) {
#if 0
            g_tree_foreach(set->tree, page_entry_unlock, NULL);
#endif
            goto retry;
        }
        assert_page_locked(pd);
        PAGE_FOR_EACH_TB(pd, tb, n) {
            if (page_trylock_add(uc, set, tb->page_addr[0]) ||
                (tb->page_addr[1] != -1 &&
                 page_trylock_add(uc, set, tb->page_addr[1]))) {
                /* drop all locks, and reacquire in order */
#if 0
                g_tree_foreach(set->tree, page_entry_unlock, NULL);
#endif
                goto retry;
            }
        }
    }
    return set;
}