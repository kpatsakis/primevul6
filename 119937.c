int wc_RsaExportKey(RsaKey* key,
                    byte* e, word32* eSz, byte* n, word32* nSz,
                    byte* d, word32* dSz, byte* p, word32* pSz,
                    byte* q, word32* qSz)
{
    int ret = BAD_FUNC_ARG;

    if (key && e && eSz && n && nSz && d && dSz && p && pSz && q && qSz)
        ret = 0;

    if (ret == 0)
        ret = RsaGetValue(&key->e, e, eSz);
    if (ret == 0)
        ret = RsaGetValue(&key->n, n, nSz);
#ifndef WOLFSSL_RSA_PUBLIC_ONLY
    if (ret == 0)
        ret = RsaGetValue(&key->d, d, dSz);
    if (ret == 0)
        ret = RsaGetValue(&key->p, p, pSz);
    if (ret == 0)
        ret = RsaGetValue(&key->q, q, qSz);
#else
    /* no private parts to key */
    if (d == NULL || p == NULL || q == NULL || dSz == NULL || pSz == NULL
            || qSz == NULL) {
        ret = BAD_FUNC_ARG;
    }
    else {
        *dSz = 0;
        *pSz = 0;
        *qSz = 0;
    }
#endif /* WOLFSSL_RSA_PUBLIC_ONLY */

    return ret;
}