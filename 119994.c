int  wc_FreeRsaKey(RsaKey* key)
{
    return FreeRsaKey_fips(key);
}