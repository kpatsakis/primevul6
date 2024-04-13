int wc_SignatureGenerate(
    enum wc_HashType hash_type, enum wc_SignatureType sig_type,
    const byte* data, word32 data_len,
    byte* sig, word32 *sig_len,
    const void* key, word32 key_len, WC_RNG* rng)
{
    return wc_SignatureGenerate_ex(hash_type, sig_type, data, data_len, sig,
        sig_len, key, key_len, rng, 1);
}