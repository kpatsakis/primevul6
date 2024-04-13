int  wc_RsaSSL_Sign(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key, WC_RNG* rng)
{
    if (in == NULL || out == NULL || key == NULL || inLen == 0) {
        return BAD_FUNC_ARG;
    }
    return RsaSSL_Sign_fips(in, inLen, out, outLen, key, rng);
}