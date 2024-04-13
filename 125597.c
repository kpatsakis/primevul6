sds createStreamIDString(streamID *id) {
    /* Optimization: pre-allocate a big enough buffer to avoid reallocs. */
    sds str = sdsnewlen(SDS_NOINIT, STREAM_ID_STR_LEN);
    sdssetlen(str, 0);
    return sdscatfmt(str,"%U-%U", id->ms,id->seq);
}