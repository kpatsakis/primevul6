int wc_RsaPSS_Sign(const byte* in, word32 inLen, byte* out, word32 outLen,
                       enum wc_HashType hash, int mgf, RsaKey* key, WC_RNG* rng)
{
    return wc_RsaPSS_Sign_ex(in, inLen, out, outLen, hash, mgf,
                                            RSA_PSS_SALT_LEN_DEFAULT, key, rng);
}