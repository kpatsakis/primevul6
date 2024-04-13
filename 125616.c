int streamParseStrictIDOrReply(client *c, robj *o, streamID *id, uint64_t missing_seq, int *seq_given) {
    return streamGenericParseIDOrReply(c,o,id,missing_seq,1,seq_given);
}