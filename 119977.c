static void wc_RsaCleanup(RsaKey* key)
{
#ifndef WOLFSSL_RSA_VERIFY_INLINE
    if (key && key->data) {
        /* make sure any allocated memory is free'd */
        if (key->dataIsAlloc) {
        #ifndef WOLFSSL_RSA_PUBLIC_ONLY
            if (key->type == RSA_PRIVATE_DECRYPT ||
                key->type == RSA_PRIVATE_ENCRYPT) {
                ForceZero(key->data, key->dataLen);
            }
        #endif
            XFREE(key->data, key->heap, DYNAMIC_TYPE_WOLF_BIGINT);
            key->dataIsAlloc = 0;
        }
        key->data = NULL;
        key->dataLen = 0;
    }
#else
    (void)key;
#endif
}