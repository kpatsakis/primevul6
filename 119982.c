int wc_InitRsaHw(RsaKey* key)
{
    CRYSError_t ret = 0;
    byte e[3];
    word32 eSz = sizeof(e);
    byte n[256];
    word32 nSz = sizeof(n);
    byte d[256];
    word32 dSz = sizeof(d);
    byte p[128];
    word32 pSz = sizeof(p);
    byte q[128];
    word32 qSz = sizeof(q);

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    ret = wc_RsaExportKey(key, e, &eSz, n, &nSz, d, &dSz, p, &pSz, q, &qSz);
    if (ret != 0)
        return MP_READ_E;

    ret = CRYS_RSA_Build_PubKey(&key->ctx.pubKey, e, eSz, n, nSz);
    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_RSA_Build_PubKey failed");
        return ret;
    }

    ret =  CRYS_RSA_Build_PrivKey(&key->ctx.privKey, d, dSz, e, eSz, n, nSz);

    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_RSA_Build_PrivKey failed");
        return ret;
    }
    key->type = RSA_PRIVATE;
    return 0;
}