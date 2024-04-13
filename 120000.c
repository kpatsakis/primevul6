int wc_InitRsaKey_Id(RsaKey* key, unsigned char* id, int len, void* heap,
                     int devId)
{
    int ret = 0;

    if (key == NULL)
        ret = BAD_FUNC_ARG;
    if (ret == 0 && (len < 0 || len > RSA_MAX_ID_LEN))
        ret = BUFFER_E;

    if (ret == 0)
        ret = wc_InitRsaKey_ex(key, heap, devId);

    if (ret == 0 && id != NULL && len != 0) {
        XMEMCPY(key->id, id, len);
        key->idLen = len;
    }

    return ret;
}