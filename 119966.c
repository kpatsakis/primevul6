int  wc_InitRsaKey_ex(RsaKey* key, void* ptr, int devId)
{
    (void)devId;
    if (key == NULL) {
        return BAD_FUNC_ARG;
    }
    return InitRsaKey_fips(key, ptr);
}