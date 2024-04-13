int streamDecrID(streamID *id) {
    int ret = C_OK;
    if (id->seq == 0) {
        if (id->ms == 0) {
            /* Special case where 'id' is the first possible streamID... */
            id->ms = id->seq = UINT64_MAX;
            ret = C_ERR;
        } else {
            id->ms--;
            id->seq = UINT64_MAX;
        }
    } else {
        id->seq--;
    }
    return ret;
}