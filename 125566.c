int streamParseIntervalIDOrReply(client *c, robj *o, streamID *id, int *exclude, uint64_t missing_seq) {
    char *p = o->ptr;
    size_t len = sdslen(p);
    int invalid = 0;
    
    if (exclude != NULL) *exclude = (len > 1 && p[0] == '(');
    if (exclude != NULL && *exclude) {
        robj *t = createStringObject(p+1,len-1);
        invalid = (streamParseStrictIDOrReply(c,t,id,missing_seq,NULL) == C_ERR);
        decrRefCount(t);
    } else 
        invalid = (streamParseIDOrReply(c,o,id,missing_seq) == C_ERR);
    if (invalid)
        return C_ERR;
    return C_OK;
}