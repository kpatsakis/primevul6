static void page_entry_destroy(gpointer p)
{
    struct page_entry *pe = p;

    // g_assert(pe->locked);
    page_unlock(pe->pd);
    g_free(pe);
}