int  wc_RsaPrivateDecrypt(const byte* in, word32 inLen, byte* out,
                                  word32 outLen, RsaKey* key)
{
    if (in == NULL || out == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }
    return RsaPrivateDecrypt_fips(in, inLen, out, outLen, key);
}