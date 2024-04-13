static bool page_is_locked(const PageDesc *pd)
{
    PageDesc *found;

    ht_pages_locked_debug_init();
    found = g_hash_table_lookup(ht_pages_locked_debug, pd);
    return !!found;
}