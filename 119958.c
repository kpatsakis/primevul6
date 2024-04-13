int wc_RsaPublicEncrypt_ex(const byte* in, word32 inLen, byte* out,
                    word32 outLen, RsaKey* key, WC_RNG* rng, int type,
                    enum wc_HashType hash, int mgf, byte* label,
                    word32 labelSz)
{
    return RsaPublicEncryptEx(in, inLen, out, outLen, key, RSA_PUBLIC_ENCRYPT,
        RSA_BLOCK_TYPE_2, type, hash, mgf, label, labelSz, 0, rng);
}