int wc_InitRsaKey_ex(RsaKey* key, void* heap, int devId)
{
    int ret = 0;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    XMEMSET(key, 0, sizeof(RsaKey));

    key->type = RSA_TYPE_UNKNOWN;
    key->state = RSA_STATE_NONE;
    key->heap = heap;
#ifndef WOLFSSL_RSA_VERIFY_INLINE
    key->dataIsAlloc = 0;
    key->data = NULL;
#endif
    key->dataLen = 0;
#ifdef WC_RSA_BLINDING
    key->rng = NULL;
#endif

#ifdef WOLF_CRYPTO_CB
    key->devId = devId;
#else
    (void)devId;
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    #ifdef WOLFSSL_CERT_GEN
        XMEMSET(&key->certSignCtx, 0, sizeof(CertSignCtx));
    #endif

    #ifdef WC_ASYNC_ENABLE_RSA
        /* handle as async */
        ret = wolfAsync_DevCtxInit(&key->asyncDev, WOLFSSL_ASYNC_MARKER_RSA,
                                                            key->heap, devId);
        if (ret != 0)
            return ret;
    #endif /* WC_ASYNC_ENABLE_RSA */
#endif /* WOLFSSL_ASYNC_CRYPT */

#ifndef WOLFSSL_RSA_PUBLIC_ONLY
    ret = mp_init_multi(&key->n, &key->e, NULL, NULL, NULL, NULL);
    if (ret != MP_OKAY)
        return ret;

#if !defined(WOLFSSL_KEY_GEN) && !defined(OPENSSL_EXTRA) && defined(RSA_LOW_MEM)
    ret = mp_init_multi(&key->d, &key->p, &key->q, NULL, NULL, NULL);
#else
    ret = mp_init_multi(&key->d, &key->p, &key->q, &key->dP, &key->dQ, &key->u);
#endif
    if (ret != MP_OKAY) {
        mp_clear(&key->n);
        mp_clear(&key->e);
        return ret;
    }
#else
    ret = mp_init(&key->n);
    if (ret != MP_OKAY)
        return ret;
    ret = mp_init(&key->e);
    if (ret != MP_OKAY) {
        mp_clear(&key->n);
        return ret;
    }
#endif

#ifdef WOLFSSL_XILINX_CRYPT
    key->pubExp = 0;
    key->mod    = NULL;
#endif

#ifdef WOLFSSL_AFALG_XILINX_RSA
    key->alFd = WC_SOCK_NOTSET;
    key->rdFd = WC_SOCK_NOTSET;
#endif

    return ret;
}