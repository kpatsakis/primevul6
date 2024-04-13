static int wc_SignatureDerEncode(enum wc_HashType hash_type, byte* hash_data,
    word32 hash_len, word32* hash_enc_len)
{
    int ret, oid;

    ret = wc_HashGetOID(hash_type);
    if (ret < 0) {
        return ret;
    }
    oid = ret;

    ret = wc_EncodeSignature(hash_data, hash_data, hash_len, oid);
    if (ret > 0) {
        *hash_enc_len = ret;
        ret = 0;
    }

    return ret;
}