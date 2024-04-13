int streamGenericParseIDOrReply(client *c, const robj *o, streamID *id, uint64_t missing_seq, int strict, int *seq_given) {
    char buf[128];
    if (sdslen(o->ptr) > sizeof(buf)-1) goto invalid;
    memcpy(buf,o->ptr,sdslen(o->ptr)+1);

    if (strict && (buf[0] == '-' || buf[0] == '+') && buf[1] == '\0')
        goto invalid;

    if (seq_given != NULL) {
        *seq_given = 1;
    }

    /* Handle the "-" and "+" special cases. */
    if (buf[0] == '-' && buf[1] == '\0') {
        id->ms = 0;
        id->seq = 0;
        return C_OK;
    } else if (buf[0] == '+' && buf[1] == '\0') {
        id->ms = UINT64_MAX;
        id->seq = UINT64_MAX;
        return C_OK;
    }

    /* Parse <ms>-<seq> form. */
    unsigned long long ms, seq;
    char *dot = strchr(buf,'-');
    if (dot) *dot = '\0';
    if (string2ull(buf,&ms) == 0) goto invalid;
    if (dot) {
        size_t seqlen = strlen(dot+1);
        if (seq_given != NULL && seqlen == 1 && *(dot + 1) == '*') {
            /* Handle the <ms>-* form. */
            seq = 0;
            *seq_given = 0;
        } else if (string2ull(dot+1,&seq) == 0) {
            goto invalid;
        }
    } else {
        seq = missing_seq;
    }
    id->ms = ms;
    id->seq = seq;
    return C_OK;

invalid:
    if (c) addReplyError(c,"Invalid stream ID specified as stream "
                           "command argument");
    return C_ERR;
}