int wc_RsaPSS_VerifyInline(byte* in, word32 inLen, byte** out,
                           enum wc_HashType hash, int mgf, RsaKey* key)
{
#ifndef WOLFSSL_PSS_SALT_LEN_DISCOVER
    return wc_RsaPSS_VerifyInline_ex(in, inLen, out, hash, mgf,
                                                 RSA_PSS_SALT_LEN_DEFAULT, key);
#else
    return wc_RsaPSS_VerifyInline_ex(in, inLen, out, hash, mgf,
                                                RSA_PSS_SALT_LEN_DISCOVER, key);
#endif
}