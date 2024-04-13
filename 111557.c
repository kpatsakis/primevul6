static bool page_trylock_add(struct uc_struct *uc, struct page_collection *set, tb_page_addr_t addr)
{
    tb_page_addr_t index = addr >> TARGET_PAGE_BITS;
    struct page_entry *pe;
    PageDesc *pd;

    pe = g_tree_lookup(set->tree, &index);
    if (pe) {
        return false;
    }

    pd = page_find(uc, index);
    if (pd == NULL) {
        return false;
    }

    pe = page_entry_new(pd, index);
    g_tree_insert(set->tree, &pe->index, pe);

    /*
     * If this is either (1) the first insertion or (2) a page whose index
     * is higher than any other so far, just lock the page and move on.
     */
    if (set->max == NULL || pe->index > set->max->index) {
        set->max = pe;
#if 0
        do_page_entry_lock(pe);
#endif
        return false;
    }
    /*
     * Try to acquire out-of-order lock; if busy, return busy so that we acquire
     * locks in order.
     */
#if 0
    return page_entry_trylock(pe);
#else
    return 0;
#endif
}