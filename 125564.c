int streamIncrID(streamID *id) {
    int ret = C_OK;
    if (id->seq == UINT64_MAX) {
        if (id->ms == UINT64_MAX) {
            /* Special case where 'id' is the last possible streamID... */
            id->ms = id->seq = 0;
            ret = C_ERR;
        } else {
            id->ms++;
            id->seq = 0;
        }
    } else {
        id->seq++;
    }
    return ret;
}