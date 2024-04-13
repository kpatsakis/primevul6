int streamParseID(const robj *o, streamID *id) {
    return streamGenericParseIDOrReply(NULL,o,id,0,0,NULL);
}