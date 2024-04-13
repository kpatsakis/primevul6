int  wc_RsaEncryptSize(RsaKey* key)
{
    if (key == NULL) {
        return BAD_FUNC_ARG;
    }
    return RsaEncryptSize_fips(key);
}