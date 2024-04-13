static int cc310_RsaPublicEncrypt(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key)
{
    CRYSError_t ret = 0;
    CRYS_RSAPrimeData_t primeData;
    int modulusSize = wc_RsaEncryptSize(key);

    /* The out buffer must be at least modulus size bytes long. */
    if (outLen < modulusSize)
        return BAD_FUNC_ARG;

    ret = CRYS_RSA_PKCS1v15_Encrypt(&wc_rndState,
                                    wc_rndGenVectFunc,
                                    &key->ctx.pubKey,
                                    &primeData,
                                    (byte*)in,
                                    inLen,
                                    out);

    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_RSA_PKCS1v15_Encrypt failed");
        return -1;
    }

    return modulusSize;
}