static void page_flush_tb(struct uc_struct *uc)
{
    int i, l1_sz = uc->v_l1_size;

    for (i = 0; i < l1_sz; i++) {
        page_flush_tb_1(uc, uc->v_l2_levels, uc->l1_map + i);
    }
}