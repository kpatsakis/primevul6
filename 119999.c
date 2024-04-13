static int RsaPublicEncryptEx(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key, int rsa_type,
                            byte pad_value, int pad_type,
                            enum wc_HashType hash, int mgf,
                            byte* label, word32 labelSz, int saltLen,
                            WC_RNG* rng)
{
    int ret, sz;

    if (in == NULL || inLen == 0 || out == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    sz = wc_RsaEncryptSize(key);
    if (sz > (int)outLen) {
        return RSA_BUFFER_E;
    }

    if (sz < RSA_MIN_PAD_SZ) {
        return WC_KEY_SIZE_E;
    }

    if (inLen > (word32)(sz - RSA_MIN_PAD_SZ)) {
#ifdef WC_RSA_NO_PADDING
        /* In the case that no padding is used the input length can and should
         * be the same size as the RSA key. */
        if (pad_type != WC_RSA_NO_PAD)
#endif
        return RSA_BUFFER_E;
    }

    switch (key->state) {
    case RSA_STATE_NONE:
    case RSA_STATE_ENCRYPT_PAD:
    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA) && \
            defined(HAVE_CAVIUM)
        if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_RSA &&
                                 pad_type != WC_RSA_PSS_PAD && key->n.raw.buf) {
            /* Async operations that include padding */
            if (rsa_type == RSA_PUBLIC_ENCRYPT &&
                                                pad_value == RSA_BLOCK_TYPE_2) {
                key->state = RSA_STATE_ENCRYPT_RES;
                key->dataLen = key->n.raw.len;
                return NitroxRsaPublicEncrypt(in, inLen, out, outLen, key);
            }
            else if (rsa_type == RSA_PRIVATE_ENCRYPT &&
                                                pad_value == RSA_BLOCK_TYPE_1) {
                key->state = RSA_STATE_ENCRYPT_RES;
                key->dataLen = key->n.raw.len;
                return NitroxRsaSSL_Sign(in, inLen, out, outLen, key);
            }
        }
    #elif defined(WOLFSSL_CRYPTOCELL)
        if (rsa_type == RSA_PUBLIC_ENCRYPT &&
                                            pad_value == RSA_BLOCK_TYPE_2) {

            return cc310_RsaPublicEncrypt(in, inLen, out, outLen, key);
        }
        else if (rsa_type == RSA_PRIVATE_ENCRYPT &&
                                         pad_value == RSA_BLOCK_TYPE_1) {
         return cc310_RsaSSL_Sign(in, inLen, out, outLen, key,
                                  cc310_hashModeRSA(hash, 0));
        }
    #endif /* WOLFSSL_CRYPTOCELL */

        key->state = RSA_STATE_ENCRYPT_PAD;
        ret = wc_RsaPad_ex(in, inLen, out, sz, pad_value, rng, pad_type, hash,
                           mgf, label, labelSz, saltLen, mp_count_bits(&key->n),
                           key->heap);
        if (ret < 0) {
            break;
        }

        key->state = RSA_STATE_ENCRYPT_EXPTMOD;
        FALL_THROUGH;

    case RSA_STATE_ENCRYPT_EXPTMOD:

        key->dataLen = outLen;
        ret = wc_RsaFunction(out, sz, out, &key->dataLen, rsa_type, key, rng);

        if (ret >= 0 || ret == WC_PENDING_E) {
            key->state = RSA_STATE_ENCRYPT_RES;
        }
        if (ret < 0) {
            break;
        }

        FALL_THROUGH;

    case RSA_STATE_ENCRYPT_RES:
        ret = key->dataLen;
        break;

    default:
        ret = BAD_STATE_E;
        break;
    }

    /* if async pending then return and skip done cleanup below */
    if (ret == WC_PENDING_E
    #ifdef WC_RSA_NONBLOCK
        || ret == FP_WOULDBLOCK
    #endif
    ) {
        return ret;
    }

    key->state = RSA_STATE_NONE;
    wc_RsaCleanup(key);

    return ret;
}