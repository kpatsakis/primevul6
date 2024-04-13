int wc_RsaPSS_Sign_ex(const byte* in, word32 inLen, byte* out, word32 outLen,
                      enum wc_HashType hash, int mgf, int saltLen, RsaKey* key,
                      WC_RNG* rng)
{
    return RsaPublicEncryptEx(in, inLen, out, outLen, key,
        RSA_PRIVATE_ENCRYPT, RSA_BLOCK_TYPE_1, WC_RSA_PSS_PAD,
        hash, mgf, NULL, 0, saltLen, rng);
}