int wc_RsaPSS_CheckPadding(const byte* in, word32 inSz, byte* sig,
                           word32 sigSz, enum wc_HashType hashType)
{
    return wc_RsaPSS_CheckPadding_ex(in, inSz, sig, sigSz, hashType, inSz, 0);
}