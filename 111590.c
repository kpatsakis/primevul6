static inline void invalidate_page_bitmap(PageDesc *p)
{
    assert_page_locked(p);

    g_free(p->code_bitmap);
    p->code_bitmap = NULL;
    p->code_write_count = 0;
}