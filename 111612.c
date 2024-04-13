void tb_invalidate_phys_page_range(struct uc_struct *uc, tb_page_addr_t start, tb_page_addr_t end)
{
    struct page_collection *pages;
    PageDesc *p;

    assert_memory_lock();

    p = page_find(uc, start >> TARGET_PAGE_BITS);
    if (p == NULL) {
        return;
    }
    pages = page_collection_lock(uc, start, end);
    tb_invalidate_phys_page_range__locked(uc, pages, p, start, end, 0);
    page_collection_unlock(pages);
}