static int RsaGetValue(mp_int* in, byte* out, word32* outSz)
{
    word32 sz;
    int ret = 0;

    /* Parameters ensured by calling function. */

    sz = (word32)mp_unsigned_bin_size(in);
    if (sz > *outSz)
        ret = RSA_BUFFER_E;

    if (ret == 0)
        ret = mp_to_unsigned_bin(in, out);

    if (ret == MP_OKAY)
        *outSz = sz;

    return ret;
}