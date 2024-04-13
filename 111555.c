void tb_invalidate_phys_range(struct uc_struct *uc, ram_addr_t start, ram_addr_t end)
{
    struct page_collection *pages;
    tb_page_addr_t next;

    assert_memory_lock();

    pages = page_collection_lock(uc, start, end);
    for (next = (start & TARGET_PAGE_MASK) + TARGET_PAGE_SIZE;
         //start < end; Unicorn: Fix possible wrap around
         (intptr_t)(end - start) > 0;
         start = next, next += TARGET_PAGE_SIZE) {
        PageDesc *pd = page_find(uc, start >> TARGET_PAGE_BITS);
        tb_page_addr_t bound = MIN(next, end);

        if (pd == NULL) {
            continue;
        }
        tb_invalidate_phys_page_range__locked(uc, pages, pd, start, bound, 0);
    }
    page_collection_unlock(pages);
}