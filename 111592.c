static void build_page_bitmap(struct uc_struct *uc, PageDesc *p)
{
    int n, tb_start, tb_end;
    TranslationBlock *tb;

    assert_page_locked(p);
    p->code_bitmap = bitmap_new(TARGET_PAGE_SIZE);

    PAGE_FOR_EACH_TB(p, tb, n) {
        /* NOTE: this is subtle as a TB may span two physical pages */
        if (n == 0) {
            /* NOTE: tb_end may be after the end of the page, but
               it is not a problem */
            tb_start = tb->pc & ~TARGET_PAGE_MASK;
            tb_end = tb_start + tb->size;
            if (tb_end > TARGET_PAGE_SIZE) {
                tb_end = TARGET_PAGE_SIZE;
             }
        } else {
            tb_start = 0;
            tb_end = ((tb->pc + tb->size) & ~TARGET_PAGE_MASK);
        }
        qemu_bitmap_set(p->code_bitmap, tb_start, tb_end - tb_start);
    }
}