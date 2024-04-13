int cc310_RsaSSL_Sign(const byte* in, word32 inLen, byte* out,
                  word32 outLen, RsaKey* key, CRYS_RSA_HASH_OpMode_t mode)
{
    CRYSError_t ret = 0;
    uint16_t actualOutLen = outLen*sizeof(byte);
    CRYS_RSAPrivUserContext_t  contextPrivate;

    ret =  CRYS_RSA_PKCS1v15_Sign(&wc_rndState,
                wc_rndGenVectFunc,
                &contextPrivate,
                &key->ctx.privKey,
                mode,
                (byte*)in,
                inLen,
                out,
                &actualOutLen);

    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_RSA_PKCS1v15_Sign failed");
        return -1;
    }
    return actualOutLen;
}