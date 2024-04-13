robj *streamTypeLookupWriteOrCreate(client *c, robj *key, int no_create) {
    robj *o = lookupKeyWrite(c->db,key);
    if (checkType(c,o,OBJ_STREAM)) return NULL;
    if (o == NULL) {
        if (no_create) {
            addReplyNull(c);
            return NULL;
        }
        o = createStreamObject();
        dbAdd(c->db,key,o);
    }
    return o;
}