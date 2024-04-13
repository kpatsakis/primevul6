int wc_SignatureGenerateHash(
    enum wc_HashType hash_type, enum wc_SignatureType sig_type,
    const byte* hash_data, word32 hash_len,
    byte* sig, word32 *sig_len,
    const void* key, word32 key_len, WC_RNG* rng)
{
    return wc_SignatureGenerateHash_ex(hash_type, sig_type, hash_data, hash_len,
        sig, sig_len, key, key_len, rng, 1);
}