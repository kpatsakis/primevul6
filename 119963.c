int  wc_RsaSSL_Verify(const byte* in, word32 inLen, byte* out,
                              word32 outLen, RsaKey* key)
{
    if (in == NULL || out == NULL || key == NULL || inLen == 0) {
        return BAD_FUNC_ARG;
    }
    return RsaSSL_Verify_fips(in, inLen, out, outLen, key);
}