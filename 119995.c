int wc_RsaSSL_Sign(const byte* in, word32 inLen, byte* out, word32 outLen,
                                                   RsaKey* key, WC_RNG* rng)
{
    return RsaPublicEncryptEx(in, inLen, out, outLen, key,
        RSA_PRIVATE_ENCRYPT, RSA_BLOCK_TYPE_1, WC_RSA_PKCSV15_PAD,
        WC_HASH_TYPE_NONE, WC_MGF1NONE, NULL, 0, 0, rng);
}