static int cc310_RSA_GenerateKeyPair(RsaKey* key, int size, long e)
{
    CRYSError_t             ret = 0;
    CRYS_RSAKGData_t        KeyGenData;
    CRYS_RSAKGFipsContext_t FipsCtx;
    byte ex[3];
    uint16_t eSz = sizeof(ex);
    byte n[256];
    uint16_t nSz = sizeof(n);

    ret = CRYS_RSA_KG_GenerateKeyPair(&wc_rndState,
                        wc_rndGenVectFunc,
                        (byte*)&e,
                        3*sizeof(uint8_t),
                        size,
                        &key->ctx.privKey,
                        &key->ctx.pubKey,
                        &KeyGenData,
                        &FipsCtx);

    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_RSA_KG_GenerateKeyPair failed");
        return ret;
    }

    ret = CRYS_RSA_Get_PubKey(&key->ctx.pubKey, ex, &eSz, n, &nSz);
    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_RSA_Get_PubKey failed");
        return ret;
    }
    ret = wc_RsaPublicKeyDecodeRaw(n, nSz, ex, eSz, key);

    key->type = RSA_PRIVATE;

    return ret;
}