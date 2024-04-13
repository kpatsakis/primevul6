static void tb_clean_internal(void **p, int x)
{
    int i;
    void **q;

    if (x <= 1) {
        for (i = 0; i < V_L2_SIZE; i++) {
            q = p[i];
            if (q) {
                g_free(q);
            }
        }
        g_free(p);
    } else {
        for (i = 0; i < V_L2_SIZE; i++) {
            q = p[i];
            if (q) {
                tb_clean_internal(q, x - 1);
            }
        }
        g_free(p);
    }
}