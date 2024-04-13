int  wc_RsaSSL_Verify_ex(const byte* in, word32 inLen, byte* out, word32 outLen,
                         RsaKey* key, int pad_type)
{
    WC_RNG* rng;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WC_RSA_BLINDING
    rng = key->rng;
#else
    rng = NULL;
#endif

    return RsaPrivateDecryptEx((byte*)in, inLen, out, outLen, NULL, key,
        RSA_PUBLIC_DECRYPT, RSA_BLOCK_TYPE_1, pad_type,
        WC_HASH_TYPE_NONE, WC_MGF1NONE, NULL, 0, 0, rng);
}