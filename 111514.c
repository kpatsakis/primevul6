static void page_flush_tb_1(struct uc_struct *uc, int level, void **lp)
{
    int i;

    if (*lp == NULL) {
        return;
    }
    if (level == 0) {
        PageDesc *pd = *lp;

        for (i = 0; i < V_L2_SIZE; ++i) {
            page_lock(&pd[i]);
            pd[i].first_tb = (uintptr_t)NULL;
            invalidate_page_bitmap(pd + i);
            page_unlock(&pd[i]);
        }
    } else {
        void **pp = *lp;

        for (i = 0; i < V_L2_SIZE; ++i) {
            page_flush_tb_1(uc, level - 1, pp + i);
        }
    }
}