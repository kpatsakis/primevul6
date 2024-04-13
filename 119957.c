int  wc_RsaPublicEncrypt(const byte* in, word32 inLen, byte* out,
                                 word32 outLen, RsaKey* key, WC_RNG* rng)
{
    if (in == NULL || out == NULL || key == NULL || rng == NULL) {
        return BAD_FUNC_ARG;
    }
    return RsaPublicEncrypt_fips(in, inLen, out, outLen, key, rng);
}