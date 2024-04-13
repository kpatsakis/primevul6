int  wc_RsaSSL_VerifyInline(byte* in, word32 inLen, byte** out, RsaKey* key)
{
    if (in == NULL || out == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }
    return RsaSSL_VerifyInline_fips(in, inLen, out, key);
}