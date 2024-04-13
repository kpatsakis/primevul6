void assert_no_pages_locked(void)
{
    ht_pages_locked_debug_init();
    g_assert(g_hash_table_size(ht_pages_locked_debug) == 0);
}