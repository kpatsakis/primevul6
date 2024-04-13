int wc_RsaPrivateDecryptInline(byte* in, word32 inLen, byte** out, RsaKey* key)
{
    WC_RNG* rng;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#else
    rng = NULL;
#endif
    return RsaPrivateDecryptEx(in, inLen, in, inLen, out, key,
        RSA_PRIVATE_DECRYPT, RSA_BLOCK_TYPE_2, WC_RSA_PKCSV15_PAD,
        WC_HASH_TYPE_NONE, WC_MGF1NONE, NULL, 0, 0, rng);
}