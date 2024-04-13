int wc_RsaPSS_VerifyCheck(byte* in, word32 inLen, byte* out, word32 outLen,
                          const byte* digest, word32 digestLen,
                          enum wc_HashType hash, int mgf,
                          RsaKey* key)
{
    int ret = 0, verify, saltLen, hLen, bits = 0;

    hLen = wc_HashGetDigestSize(hash);
    if (hLen < 0)
        return hLen;
    if ((word32)hLen != digestLen)
        return BAD_FUNC_ARG;

    saltLen = hLen;
    #ifdef WOLFSSL_SHA512
        /* See FIPS 186-4 section 5.5 item (e). */
        bits = mp_count_bits(&key->n);
        if (bits == 1024 && hLen == WC_SHA512_DIGEST_SIZE)
            saltLen = RSA_PSS_SALT_MAX_SZ;
    #endif

    verify = wc_RsaPSS_Verify_ex(in, inLen, out, outLen, hash,
                                 mgf, saltLen, key);
    if (verify > 0)
        ret = wc_RsaPSS_CheckPadding_ex(digest, digestLen, out, verify,
                                        hash, saltLen, bits);
    if (ret == 0)
        ret = verify;

    return ret;
}