static int cc310_RsaPublicDecrypt(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key)
{
    CRYSError_t ret = 0;
    CRYS_RSAPrimeData_t primeData;
    uint16_t actualOutLen = outLen;

    ret = CRYS_RSA_PKCS1v15_Decrypt(&key->ctx.privKey,
                                    &primeData,
                                    (byte*)in,
                                    inLen,
                                    out,
                                    &actualOutLen);

    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_RSA_PKCS1v15_Decrypt failed");
        return -1;
    }
    return actualOutLen;
}