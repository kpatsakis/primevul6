static void page_lock_pair(struct uc_struct *uc, PageDesc **ret_p1, tb_page_addr_t phys1,
                           PageDesc **ret_p2, tb_page_addr_t phys2, int alloc)
{
    PageDesc *p1, *p2;
    tb_page_addr_t page1;
    tb_page_addr_t page2;

    assert_memory_lock();
    g_assert(phys1 != -1);

    page1 = phys1 >> TARGET_PAGE_BITS;
    page2 = phys2 >> TARGET_PAGE_BITS;

    p1 = page_find_alloc(uc, page1, alloc);
    if (ret_p1) {
        *ret_p1 = p1;
    }
    if (likely(phys2 == -1)) {
        page_lock(p1);
        return;
    } else if (page1 == page2) {
        page_lock(p1);
        if (ret_p2) {
            *ret_p2 = p1;
        }
        return;
    }
    p2 = page_find_alloc(uc, page2, alloc);
    if (ret_p2) {
        *ret_p2 = p2;
    }
    if (page1 < page2) {
        page_lock(p1);
        page_lock(p2);
    } else {
        page_lock(p2);
        page_lock(p1);
    }
}