int  wc_InitRsaKey(RsaKey* key, void* ptr)
{
    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    return InitRsaKey_fips(key, ptr);
}