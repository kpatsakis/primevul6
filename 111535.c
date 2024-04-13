static PageDesc *page_find_alloc(struct uc_struct *uc, tb_page_addr_t index, int alloc)
{
    PageDesc *pd;
    void **lp;
    int i;

    /* Level 1.  Always allocated.  */
    lp = uc->l1_map + ((index >> uc->v_l1_shift) & (uc->v_l1_size - 1));

    /* Level 2..N-1.  */
    for (i = uc->v_l2_levels; i > 0; i--) {
        void **p = *lp;

        if (p == NULL) {
            void *existing;

            if (!alloc) {
                return NULL;
            }
            p = g_new0(void *, V_L2_SIZE);
            existing = *lp;
            if (*lp == NULL) {
                *lp = p;
            }
            if (unlikely(existing)) {
                g_free(p);
                p = existing;
            }
        }

        lp = p + ((index >> (i * V_L2_BITS)) & (V_L2_SIZE - 1));
    }

    pd = *lp;
    if (pd == NULL) {
        void *existing;

        if (!alloc) {
            return NULL;
        }
        pd = g_new0(PageDesc, V_L2_SIZE);
        existing = *lp;
        if (*lp == NULL) {
            *lp = pd;
        }
        if (unlikely(existing)) {
            g_free(pd);
            pd = existing;
        }
    }

    return pd + (index & (V_L2_SIZE - 1));
}