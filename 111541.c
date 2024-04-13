void tb_invalidate_phys_page_fast(struct uc_struct *uc, struct page_collection *pages,
                                  tb_page_addr_t start, int len,
                                  uintptr_t retaddr)
{
    PageDesc *p;

    assert_memory_lock();

    p = page_find(uc, start >> TARGET_PAGE_BITS);
    if (!p) {
        return;
    }

    assert_page_locked(p);
    if (!p->code_bitmap &&
        ++p->code_write_count >= SMC_BITMAP_USE_THRESHOLD) {
        build_page_bitmap(uc, p);
    }
    if (p->code_bitmap) {
        unsigned int nr;
        unsigned long b;

        nr = start & ~TARGET_PAGE_MASK;
        b = p->code_bitmap[BIT_WORD(nr)] >> (nr & (BITS_PER_LONG - 1));
        if (b & ((1 << len) - 1)) {
            goto do_invalidate;
        }
    } else {
    do_invalidate:
        tb_invalidate_phys_page_range__locked(uc, pages, p, start, start + len,
                                              retaddr);
    }
}