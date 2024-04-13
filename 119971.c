int wc_RsaEncryptSize(RsaKey* key)
{
    int ret;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    ret = mp_unsigned_bin_size(&key->n);

#ifdef WOLF_CRYPTO_CB
    if (ret == 0 && key->devId != INVALID_DEVID) {
        ret = 2048/8; /* hardware handles, use 2048-bit as default */
    }
#endif

    return ret;
}