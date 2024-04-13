void xrangeGenericCommand(client *c, int rev) {
    robj *o;
    stream *s;
    streamID startid, endid;
    long long count = -1;
    robj *startarg = rev ? c->argv[3] : c->argv[2];
    robj *endarg = rev ? c->argv[2] : c->argv[3];
    int startex = 0, endex = 0;
    
    /* Parse start and end IDs. */
    if (streamParseIntervalIDOrReply(c,startarg,&startid,&startex,0) != C_OK)
        return;
    if (startex && streamIncrID(&startid) != C_OK) {
        addReplyError(c,"invalid start ID for the interval");
        return;
    }
    if (streamParseIntervalIDOrReply(c,endarg,&endid,&endex,UINT64_MAX) != C_OK)
        return;
    if (endex && streamDecrID(&endid) != C_OK) {
        addReplyError(c,"invalid end ID for the interval");
        return;
    }

    /* Parse the COUNT option if any. */
    if (c->argc > 4) {
        for (int j = 4; j < c->argc; j++) {
            int additional = c->argc-j-1;
            if (strcasecmp(c->argv[j]->ptr,"COUNT") == 0 && additional >= 1) {
                if (getLongLongFromObjectOrReply(c,c->argv[j+1],&count,NULL)
                    != C_OK) return;
                if (count < 0) count = 0;
                j++; /* Consume additional arg. */
            } else {
                addReplyErrorObject(c,shared.syntaxerr);
                return;
            }
        }
    }

    /* Return the specified range to the user. */
    if ((o = lookupKeyReadOrReply(c,c->argv[1],shared.emptyarray)) == NULL ||
         checkType(c,o,OBJ_STREAM)) return;

    s = o->ptr;

    if (count == 0) {
        addReplyNullArray(c);
    } else {
        if (count == -1) count = 0;
        streamReplyWithRange(c,s,&startid,&endid,count,rev,NULL,NULL,0,NULL);
    }
}