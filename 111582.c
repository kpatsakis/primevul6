page_entry_new(PageDesc *pd, tb_page_addr_t index)
{
    struct page_entry *pe = g_malloc(sizeof(*pe));

    pe->index = index;
    pe->pd = pd;
    // pe->locked = false;
    return pe;
}