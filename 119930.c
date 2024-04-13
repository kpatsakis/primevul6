int wc_RsaPSS_VerifyInline_ex(byte* in, word32 inLen, byte** out,
                              enum wc_HashType hash, int mgf, int saltLen,
                              RsaKey* key)
{
    WC_RNG* rng;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#else
    rng = NULL;
#endif
    return RsaPrivateDecryptEx(in, inLen, in, inLen, out, key,
        RSA_PUBLIC_DECRYPT, RSA_BLOCK_TYPE_1, WC_RSA_PSS_PAD,
        hash, mgf, NULL, 0, saltLen, rng);
}