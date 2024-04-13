static int RsaPrivateDecryptEx(byte* in, word32 inLen, byte* out,
                            word32 outLen, byte** outPtr, RsaKey* key,
                            int rsa_type, byte pad_value, int pad_type,
                            enum wc_HashType hash, int mgf,
                            byte* label, word32 labelSz, int saltLen,
                            WC_RNG* rng)
{
    int ret = RSA_WRONG_TYPE_E;
    byte* pad = NULL;

    if (in == NULL || inLen == 0 || out == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    switch (key->state) {
    case RSA_STATE_NONE:
        key->dataLen = inLen;

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA) && \
            defined(HAVE_CAVIUM)
        /* Async operations that include padding */
        if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_RSA &&
                                                   pad_type != WC_RSA_PSS_PAD) {
#ifndef WOLFSSL_RSA_PUBLIC_ONLY
            if (rsa_type == RSA_PRIVATE_DECRYPT &&
                                                pad_value == RSA_BLOCK_TYPE_2) {
                key->state = RSA_STATE_DECRYPT_RES;
                key->data = NULL;
                return NitroxRsaPrivateDecrypt(in, inLen, out, &key->dataLen,
                                               key);
#endif
            }
            else if (rsa_type == RSA_PUBLIC_DECRYPT &&
                                                pad_value == RSA_BLOCK_TYPE_1) {
                key->state = RSA_STATE_DECRYPT_RES;
                key->data = NULL;
                return NitroxRsaSSL_Verify(in, inLen, out, &key->dataLen, key);
            }
        }
    #elif defined(WOLFSSL_CRYPTOCELL)
        if (rsa_type == RSA_PRIVATE_DECRYPT &&
                                            pad_value == RSA_BLOCK_TYPE_2) {
            ret = cc310_RsaPublicDecrypt(in, inLen, out, outLen, key);
            if (outPtr != NULL)
                *outPtr = out; /* for inline */
            return ret;
        }
        else if (rsa_type == RSA_PUBLIC_DECRYPT &&
                                            pad_value == RSA_BLOCK_TYPE_1) {
            return cc310_RsaSSL_Verify(in, inLen, out, key,
                                       cc310_hashModeRSA(hash, 0));
        }
    #endif /* WOLFSSL_CRYPTOCELL */


#if !defined(WOLFSSL_RSA_VERIFY_ONLY) && !defined(WOLFSSL_RSA_VERIFY_INLINE)
        /* verify the tmp ptr is NULL, otherwise indicates bad state */
        if (key->data != NULL) {
            ret = BAD_STATE_E;
            break;
        }

        /* if not doing this inline then allocate a buffer for it */
        if (outPtr == NULL) {
            key->data = (byte*)XMALLOC(inLen, key->heap,
                                                      DYNAMIC_TYPE_WOLF_BIGINT);
            key->dataIsAlloc = 1;
            if (key->data == NULL) {
                ret = MEMORY_E;
                break;
            }
            XMEMCPY(key->data, in, inLen);
        }
        else {
            key->data = out;
        }
#endif

        key->state = RSA_STATE_DECRYPT_EXPTMOD;
        FALL_THROUGH;

    case RSA_STATE_DECRYPT_EXPTMOD:
#if !defined(WOLFSSL_RSA_VERIFY_ONLY) && !defined(WOLFSSL_RSA_VERIFY_INLINE)
        ret = wc_RsaFunction(key->data, inLen, key->data, &key->dataLen,
                                                            rsa_type, key, rng);
#else
        ret = wc_RsaFunction(in, inLen, out, &key->dataLen, rsa_type, key, rng);
#endif

        if (ret >= 0 || ret == WC_PENDING_E) {
            key->state = RSA_STATE_DECRYPT_UNPAD;
        }
        if (ret < 0) {
            break;
        }

        FALL_THROUGH;

    case RSA_STATE_DECRYPT_UNPAD:
#if !defined(WOLFSSL_RSA_VERIFY_ONLY) && !defined(WOLFSSL_RSA_VERIFY_INLINE)
        ret = wc_RsaUnPad_ex(key->data, key->dataLen, &pad, pad_value, pad_type,
                             hash, mgf, label, labelSz, saltLen,
                             mp_count_bits(&key->n), key->heap);
#else
        ret = wc_RsaUnPad_ex(out, key->dataLen, &pad, pad_value, pad_type, hash,
                             mgf, label, labelSz, saltLen,
                             mp_count_bits(&key->n), key->heap);
#endif
        if (rsa_type == RSA_PUBLIC_DECRYPT && ret > (int)outLen)
            ret = RSA_BUFFER_E;
        else if (ret >= 0 && pad != NULL) {
#if !defined(WOLFSSL_RSA_VERIFY_ONLY) && !defined(WOLFSSL_RSA_VERIFY_INLINE)
            signed char c;
#endif

            /* only copy output if not inline */
            if (outPtr == NULL) {
#if !defined(WOLFSSL_RSA_VERIFY_ONLY) && !defined(WOLFSSL_RSA_VERIFY_INLINE)
                if (rsa_type == RSA_PRIVATE_DECRYPT) {
                    word32 i, j;
                    int start = (int)((size_t)pad - (size_t)key->data);

                    for (i = 0, j = 0; j < key->dataLen; j++) {
                        out[i] = key->data[j];
                        c  = ctMaskGTE(j, start);
                        c &= ctMaskLT(i, outLen);
                        /* 0 - no add, -1 add */
                        i += (word32)((byte)(-c));
                    }
                }
                else
#endif
                {
                    XMEMCPY(out, pad, ret);
                }
            }
            else
                *outPtr = pad;

#if !defined(WOLFSSL_RSA_VERIFY_ONLY)
            ret = ctMaskSelInt(ctMaskLTE(ret, outLen), ret, RSA_BUFFER_E);
            ret = ctMaskSelInt(ctMaskNotEq(ret, 0), ret, RSA_BUFFER_E);
#else
            if (outLen < (word32)ret)
                ret = RSA_BUFFER_E;
#endif
        }

        key->state = RSA_STATE_DECRYPT_RES;
        FALL_THROUGH;

    case RSA_STATE_DECRYPT_RES:
    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA) && \
            defined(HAVE_CAVIUM)
        if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_RSA &&
                                                   pad_type != WC_RSA_PSS_PAD) {
            if (ret > 0) {
                /* convert result */
                byte* dataLen = (byte*)&key->dataLen;
                ret = (dataLen[0] << 8) | (dataLen[1]);

                if (outPtr)
                    *outPtr = in;
            }
        }
    #endif
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