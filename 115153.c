int wc_SignatureVerifyHash(
    enum wc_HashType hash_type, enum wc_SignatureType sig_type,
    const byte* hash_data, word32 hash_len,
    const byte* sig, word32 sig_len,
    const void* key, word32 key_len)
{
    int ret;

    /* Check arguments */
    if (hash_data == NULL || hash_len <= 0 ||
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
    ret = 0;

    /* Verify signature using hash */
    switch (sig_type) {
        case WC_SIGNATURE_TYPE_ECC:
        {
#if defined(HAVE_ECC) && defined(HAVE_ECC_VERIFY)
            int is_valid_sig = 0;

            /* Perform verification of signature using provided ECC key */
            do {
            #ifdef WOLFSSL_ASYNC_CRYPT
                ret = wc_AsyncWait(ret, &((ecc_key*)key)->asyncDev,
                    WC_ASYNC_FLAG_CALL_AGAIN);
            #endif
            if (ret >= 0)
                ret = wc_ecc_verify_hash(sig, sig_len, hash_data, hash_len,
                    &is_valid_sig, (ecc_key*)key);
            } while (ret == WC_PENDING_E);
            if (ret != 0 || is_valid_sig != 1) {
                ret = SIG_VERIFY_E;
            }
#else
            ret = SIG_TYPE_E;
#endif
            break;
        }

        case WC_SIGNATURE_TYPE_RSA_W_ENC:
        case WC_SIGNATURE_TYPE_RSA:
        {
#ifndef NO_RSA
#if defined(WOLFSSL_CRYPTOCELL)
        /* the signature must propagate to the cryptocell to get verfied */
        ret = wc_RsaSSL_Verify(hash_data, hash_len, (byte*)sig, sig_len, (RsaKey*)key);
        if (ret != 0) {
            WOLFSSL_MSG("RSA Signature Verify difference!");
            ret = SIG_VERIFY_E;
        }

#else /* WOLFSSL_CRYPTOCELL */

            word32 plain_len = hash_len;
            byte *plain_data;

            /* Make sure the plain text output is at least key size */
            if (plain_len < sig_len) {
                plain_len = sig_len;
            }
            plain_data = (byte*)XMALLOC(plain_len, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            if (plain_data) {
                /* Perform verification of signature using provided RSA key */
                do {
                #ifdef WOLFSSL_ASYNC_CRYPT
                    ret = wc_AsyncWait(ret, &((RsaKey*)key)->asyncDev,
                        WC_ASYNC_FLAG_CALL_AGAIN);
                #endif
                if (ret >= 0)
                    ret = wc_RsaSSL_Verify(sig, sig_len, plain_data,
                        plain_len, (RsaKey*)key);
                } while (ret == WC_PENDING_E);
                if (ret >= 0) {
                    if ((word32)ret == hash_len &&
                            XMEMCMP(plain_data, hash_data, hash_len) == 0) {
                        ret = 0; /* Success */
                    }
                    else {
                        WOLFSSL_MSG("RSA Signature Verify difference!");
                        ret = SIG_VERIFY_E;
                    }
                }
                XFREE(plain_data, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            }
            else {
                ret = MEMORY_E;
            }
#endif /* !WOLFSSL_CRYPTOCELL */
#else
            ret = SIG_TYPE_E;
#endif
            break;
        }

        case WC_SIGNATURE_TYPE_NONE:
        default:
            ret = BAD_FUNC_ARG;
            break;
    }

    return ret;
}