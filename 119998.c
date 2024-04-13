int wc_RsaFlattenPublicKey(RsaKey* key, byte* e, word32* eSz, byte* n,
                                                                   word32* nSz)
{
    int sz, ret;

    if (key == NULL || e == NULL || eSz == NULL || n == NULL || nSz == NULL) {
        return BAD_FUNC_ARG;
    }

    sz = mp_unsigned_bin_size(&key->e);
    if ((word32)sz > *eSz)
        return RSA_BUFFER_E;
    ret = mp_to_unsigned_bin(&key->e, e);
    if (ret != MP_OKAY)
        return ret;
    *eSz = (word32)sz;

    sz = wc_RsaEncryptSize(key);
    if ((word32)sz > *nSz)
        return RSA_BUFFER_E;
    ret = mp_to_unsigned_bin(&key->n, n);
    if (ret != MP_OKAY)
        return ret;
    *nSz = (word32)sz;

    return 0;
}