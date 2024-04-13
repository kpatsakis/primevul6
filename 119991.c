int wc_FreeRsaKey(RsaKey* key)
{
    int ret = 0;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    wc_RsaCleanup(key);

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA)
    wolfAsync_DevCtxFree(&key->asyncDev, WOLFSSL_ASYNC_MARKER_RSA);
#endif

#ifndef WOLFSSL_RSA_PUBLIC_ONLY
    if (key->type == RSA_PRIVATE) {
#if defined(WOLFSSL_KEY_GEN) || defined(OPENSSL_EXTRA) || !defined(RSA_LOW_MEM)
        mp_forcezero(&key->u);
        mp_forcezero(&key->dQ);
        mp_forcezero(&key->dP);
#endif
        mp_forcezero(&key->q);
        mp_forcezero(&key->p);
        mp_forcezero(&key->d);
    }
    /* private part */
#if defined(WOLFSSL_KEY_GEN) || defined(OPENSSL_EXTRA) || !defined(RSA_LOW_MEM)
    mp_clear(&key->u);
    mp_clear(&key->dQ);
    mp_clear(&key->dP);
#endif
    mp_clear(&key->q);
    mp_clear(&key->p);
    mp_clear(&key->d);
#endif /* WOLFSSL_RSA_PUBLIC_ONLY */

    /* public part */
    mp_clear(&key->e);
    mp_clear(&key->n);

#ifdef WOLFSSL_XILINX_CRYPT
    XFREE(key->mod, key->heap, DYNAMIC_TYPE_KEY);
    key->mod = NULL;
#endif

#ifdef WOLFSSL_AFALG_XILINX_RSA
    /* make sure that sockets are closed on cleanup */
    if (key->alFd > 0) {
        close(key->alFd);
        key->alFd = WC_SOCK_NOTSET;
    }
    if (key->rdFd > 0) {
        close(key->rdFd);
        key->rdFd = WC_SOCK_NOTSET;
    }
#endif

    return ret;
}