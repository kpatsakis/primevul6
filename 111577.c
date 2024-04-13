static void page_lock__debug(PageDesc *pd)
{
    ht_pages_locked_debug_init();
    g_assert(!page_is_locked(pd));
    g_hash_table_insert(ht_pages_locked_debug, pd, pd);
}