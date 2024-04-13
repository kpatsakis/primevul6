int wc_SignatureVerify(
    enum wc_HashType hash_type, enum wc_SignatureType sig_type,
    const byte* data, word32 data_len,
    const byte* sig, word32 sig_len,
    const void* key, word32 key_len)
{
    int ret;
    word32 hash_len, hash_enc_len;
#ifdef WOLFSSL_SMALL_STACK
    byte *hash_data;
#else
    byte hash_data[MAX_DER_DIGEST_SZ];
#endif

    /* Check arguments */
    if (data == NULL || data_len <= 0 ||
        sig == NULL || sig_len <= 0 ||
        key == NULL || key_len <= 0) {
        return BAD_FUNC_ARG;
    }

    /* Validate signature len (1 to max is okay) */
    if ((int)sig_len > wc_SignatureGetSize(sig_type, key, key_len)) {
        WOLFSSL_MSG("wc_SignatureVerify: Invalid sig type/len");
        return BAD_FUNC_ARG;
    }

    /* Validate hash size */
    ret = wc_HashGetDigestSize(hash_type);
    if (ret < 0) {
        WOLFSSL_MSG("wc_SignatureVerify: Invalid hash type/len");
        return ret;
    }
    hash_enc_len = hash_len = ret;

#ifndef NO_RSA
    if (sig_type == WC_SIGNATURE_TYPE_RSA_W_ENC) {
        /* For RSA with ASN.1 encoding include room */
        hash_enc_len += MAX_DER_DIGEST_ASN_SZ;
    }
#endif

#ifdef WOLFSSL_SMALL_STACK
    /* Allocate temporary buffer for hash data */
    hash_data = (byte*)XMALLOC(hash_enc_len, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (hash_data == NULL) {
        return MEMORY_E;
    }
#endif

    /* Perform hash of data */
    ret = wc_Hash(hash_type, data, data_len, hash_data, hash_len);
    if (ret == 0) {
        /* Handle RSA with DER encoding */
        if (sig_type == WC_SIGNATURE_TYPE_RSA_W_ENC) {
        #if defined(NO_RSA) || defined(NO_ASN)
            ret = SIG_TYPE_E;
        #else
            ret = wc_SignatureDerEncode(hash_type, hash_data, hash_len,
                &hash_enc_len);
        #endif
        }

        if (ret == 0) {
            /* Verify signature using hash */
            ret = wc_SignatureVerifyHash(hash_type, sig_type,
                hash_data, hash_enc_len, sig, sig_len, key, key_len);
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(hash_data, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}