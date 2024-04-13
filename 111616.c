static void ht_pages_locked_debug_init(void)
{
    if (ht_pages_locked_debug) {
        return;
    }
    ht_pages_locked_debug = g_hash_table_new(NULL, NULL);
}