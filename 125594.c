int streamParseIDOrReply(client *c, robj *o, streamID *id, uint64_t missing_seq) {
    return streamGenericParseIDOrReply(c,o,id,missing_seq,0,NULL);
}