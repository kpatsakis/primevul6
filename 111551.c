static void page_unlock__debug(const PageDesc *pd)
{
    bool removed;

    ht_pages_locked_debug_init();
    g_assert(page_is_locked(pd));
    removed = g_hash_table_remove(ht_pages_locked_debug, pd);
    g_assert(removed);
}