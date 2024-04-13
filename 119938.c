int wc_CheckProbablePrime_ex(const byte* pRaw, word32 pRawSz,
                          const byte* qRaw, word32 qRawSz,
                          const byte* eRaw, word32 eRawSz,
                          int nlen, int* isPrime, WC_RNG* rng)
{
    mp_int p, q, e;
    mp_int* Q = NULL;
    int ret;

    if (pRaw == NULL || pRawSz == 0 ||
        eRaw == NULL || eRawSz == 0 ||
        isPrime == NULL) {

        return BAD_FUNC_ARG;
    }

    if ((qRaw != NULL && qRawSz == 0) || (qRaw == NULL && qRawSz != 0))
        return BAD_FUNC_ARG;

    ret = mp_init_multi(&p, &q, &e, NULL, NULL, NULL);

    if (ret == MP_OKAY)
        ret = mp_read_unsigned_bin(&p, pRaw, pRawSz);

    if (ret == MP_OKAY) {
        if (qRaw != NULL) {
            ret = mp_read_unsigned_bin(&q, qRaw, qRawSz);
            if (ret == MP_OKAY)
                Q = &q;
        }
    }

    if (ret == MP_OKAY)
        ret = mp_read_unsigned_bin(&e, eRaw, eRawSz);

    if (ret == MP_OKAY)
        ret = _CheckProbablePrime(&p, Q, &e, nlen, isPrime, rng);

    ret = (ret == MP_OKAY) ? 0 : PRIME_GEN_E;

    mp_clear(&p);
    mp_clear(&q);
    mp_clear(&e);

    return ret;
}