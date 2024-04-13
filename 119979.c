int wc_RsaFlattenPublicKey(RsaKey* key, byte* a, word32* aSz, byte* b,
                           word32* bSz)
{

    /* not specified as fips so not needing _fips */
    return RsaFlattenPublicKey(key, a, aSz, b, bSz);
}