int cc310_RsaSSL_Verify(const byte* in, word32 inLen, byte* sig,
                               RsaKey* key, CRYS_RSA_HASH_OpMode_t mode)
{
    CRYSError_t ret = 0;
    CRYS_RSAPubUserContext_t contextPub;

    /* verify the signature in the sig pointer */
    ret =  CRYS_RSA_PKCS1v15_Verify(&contextPub,
                &key->ctx.pubKey,
                mode,
                (byte*)in,
                inLen,
                sig);

    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_RSA_PKCS1v15_Verify failed");
        return -1;
    }

    return ret;
}