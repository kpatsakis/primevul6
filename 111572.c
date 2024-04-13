static void do_page_entry_lock(struct page_entry *pe)
{
    page_lock(pe->pd);
    g_assert(!pe->locked);
    pe->locked = true;
}