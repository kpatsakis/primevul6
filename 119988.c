int wc_RsaPSS_Verify(byte* in, word32 inLen, byte* out, word32 outLen,
                     enum wc_HashType hash, int mgf, RsaKey* key)
{
#ifndef WOLFSSL_PSS_SALT_LEN_DISCOVER
    return wc_RsaPSS_Verify_ex(in, inLen, out, outLen, hash, mgf,
                                                 RSA_PSS_SALT_LEN_DEFAULT, key);
#else
    return wc_RsaPSS_Verify_ex(in, inLen, out, outLen, hash, mgf,
                                                RSA_PSS_SALT_LEN_DISCOVER, key);
#endif
}