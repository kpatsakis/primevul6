int  wc_RsaPrivateDecryptInline(byte* in, word32 inLen, byte** out,
                                        RsaKey* key)
{
    if (in == NULL || out == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }
    return RsaPrivateDecryptInline_fips(in, inLen, out, key);
}