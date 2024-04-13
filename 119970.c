static int wc_CompareDiffPQ(mp_int* p, mp_int* q, int size)
{
    mp_int c, d;
    int ret;

    if (p == NULL || q == NULL)
        return BAD_FUNC_ARG;

    ret = mp_init_multi(&c, &d, NULL, NULL, NULL, NULL);

    /* c = 2^((size/2)-100) */
    if (ret == 0)
        ret = mp_2expt(&c, (size/2)-100);

    /* d = |p-q| */
    if (ret == 0)
        ret = mp_sub(p, q, &d);

    if (ret == 0)
        ret = mp_abs(&d, &d);

    /* compare */
    if (ret == 0)
        ret = mp_cmp(&d, &c);

    if (ret == MP_GT)
        ret = MP_OKAY;

    mp_clear(&d);
    mp_clear(&c);

    return ret;
}