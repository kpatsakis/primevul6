int wc_RsaPrivateDecrypt_ex(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key, int type,
                            enum wc_HashType hash, int mgf, byte* label,
                            word32 labelSz)
{
    WC_RNG* rng;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#else
    rng = NULL;
#endif
    return RsaPrivateDecryptEx((byte*)in, inLen, out, outLen, NULL, key,
        RSA_PRIVATE_DECRYPT, RSA_BLOCK_TYPE_2, type, hash, mgf, label,
        labelSz, 0, rng);
}