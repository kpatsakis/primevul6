int wc_RsaFunction(const byte* in, word32 inLen, byte* out,
                          word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
    int ret = 0;

    if (key == NULL || in == NULL || inLen == 0 || out == NULL ||
            outLen == NULL || *outLen == 0 || type == RSA_TYPE_UNKNOWN) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLF_CRYPTO_CB
    if (key->devId != INVALID_DEVID) {
        ret = wc_CryptoCb_Rsa(in, inLen, out, outLen, type, key, rng);
        if (ret != CRYPTOCB_UNAVAILABLE)
            return ret;
        /* fall-through when unavailable */
        ret = 0; /* reset error code and try using software */
    }
#endif

#ifndef TEST_UNPAD_CONSTANT_TIME
#ifndef NO_RSA_BOUNDS_CHECK
    if (type == RSA_PRIVATE_DECRYPT &&
        key->state == RSA_STATE_DECRYPT_EXPTMOD) {

        /* Check that 1 < in < n-1. (Requirement of 800-56B.) */
#ifdef WOLFSSL_SMALL_STACK
        mp_int* c;
#else
        mp_int c[1];
#endif

#ifdef WOLFSSL_SMALL_STACK
        c = (mp_int*)XMALLOC(sizeof(mp_int), key->heap, DYNAMIC_TYPE_RSA);
        if (c == NULL)
            ret = MEMORY_E;
#endif

        if (mp_init(c) != MP_OKAY)
            ret = MP_INIT_E;
        if (ret == 0) {
            if (mp_read_unsigned_bin(c, in, inLen) != 0)
                ret = MP_READ_E;
        }
        if (ret == 0) {
            /* check c > 1 */
            if (mp_cmp_d(c, 1) != MP_GT)
                ret = RSA_OUT_OF_RANGE_E;
        }
        if (ret == 0) {
            /* add c+1 */
            if (mp_add_d(c, 1, c) != MP_OKAY)
                ret = MP_ADD_E;
        }
        if (ret == 0) {
            /* check c+1 < n */
            if (mp_cmp(c, &key->n) != MP_LT)
                ret = RSA_OUT_OF_RANGE_E;
        }
        mp_clear(c);

#ifdef WOLFSSL_SMALL_STACK
        XFREE(c, key->heap, DYNAMIC_TYPE_RSA);
#endif

        if (ret != 0)
            return ret;
    }
#endif /* NO_RSA_BOUNDS_CHECK */
#endif

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA)
    if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_RSA &&
                                                        key->n.raw.len > 0) {
        ret = wc_RsaFunctionAsync(in, inLen, out, outLen, type, key, rng);
    }
    else
#endif
#ifdef WC_RSA_NONBLOCK
    if (key->nb) {
        ret = wc_RsaFunctionNonBlock(in, inLen, out, outLen, type, key);
    }
    else
#endif
    {
        ret = wc_RsaFunctionSync(in, inLen, out, outLen, type, key, rng);
    }

    /* handle error */
    if (ret < 0 && ret != WC_PENDING_E
    #ifdef WC_RSA_NONBLOCK
        && ret != FP_WOULDBLOCK
    #endif
    ) {
        if (ret == MP_EXPTMOD_E) {
            /* This can happen due to incorrectly set FP_MAX_BITS or missing XREALLOC */
            WOLFSSL_MSG("RSA_FUNCTION MP_EXPTMOD_E: memory/config problem");
        }

        key->state = RSA_STATE_NONE;
        wc_RsaCleanup(key);
    }

    return ret;
}