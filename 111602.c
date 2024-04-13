void tb_cleanup(struct uc_struct *uc)
{
    int i, x;
    void **p;

    if (uc) {
        if (uc->l1_map) {
            x = uc->v_l2_levels;
            if (x <= 0) {
                for (i = 0; i < uc->v_l1_size; i++) {
                    p = uc->l1_map[i];
                    if (p) {
                        g_free(p);
                        uc->l1_map[i] = NULL;
                    }
                }
            } else {
                for (i = 0; i < uc->v_l1_size; i++) {
                    p = uc->l1_map[i];
                    if (p) {
                        tb_clean_internal(p, x);
                        uc->l1_map[i] = NULL;
                    }
                }
            }
        }
    }
}