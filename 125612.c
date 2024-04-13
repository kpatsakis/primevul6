void xsetidCommand(client *c) {
    streamID id, max_xdel_id = {0, 0};
    long long entries_added = -1;

    if (streamParseStrictIDOrReply(c,c->argv[2],&id,0,NULL) != C_OK)
        return;

    int i = 3;
    while (i < c->argc) {
        int moreargs = (c->argc-1) - i; /* Number of additional arguments. */
        char *opt = c->argv[i]->ptr;
        if (!strcasecmp(opt,"ENTRIESADDED") && moreargs) {
            if (getLongLongFromObjectOrReply(c,c->argv[i+1],&entries_added,NULL) != C_OK) {
                return;
            } else if (entries_added < 0) {
                addReplyError(c,"entries_added must be positive");
                return;
            }
            i += 2;
        } else if (!strcasecmp(opt,"MAXDELETEDID") && moreargs) {
            if (streamParseStrictIDOrReply(c,c->argv[i+1],&max_xdel_id,0,NULL) != C_OK) {
                return;
            } else if (streamCompareID(&id,&max_xdel_id) < 0) {
                addReplyError(c,"The ID specified in XSETID is smaller than the provided max_deleted_entry_id");
                return;
            }
            i += 2;
        } else {
            addReplyErrorObject(c,shared.syntaxerr);
            return;
        }
    }

    robj *o = lookupKeyWriteOrReply(c,c->argv[1],shared.nokeyerr);
    if (o == NULL || checkType(c,o,OBJ_STREAM)) return;
    stream *s = o->ptr;

    /* If the stream has at least one item, we want to check that the user
     * is setting a last ID that is equal or greater than the current top
     * item, otherwise the fundamental ID monotonicity assumption is violated. */
    if (s->length > 0) {
        streamID maxid;
        streamLastValidID(s,&maxid);

        if (streamCompareID(&id,&maxid) < 0) {
            addReplyError(c,"The ID specified in XSETID is smaller than the target stream top item");
            return;
        }

        /* If an entries_added was provided, it can't be lower than the length. */
        if (entries_added != -1 && s->length > (uint64_t)entries_added) {
            addReplyError(c,"The entries_added specified in XSETID is smaller than the target stream length");
            return;
        }
    }

    s->last_id = id;
    if (entries_added != -1)
        s->entries_added = entries_added;
    if (!streamIDEqZero(&max_xdel_id))
        s->max_deleted_entry_id = max_xdel_id;
    addReply(c,shared.ok);
    server.dirty++;
    notifyKeyspaceEvent(NOTIFY_STREAM,"xsetid",c->argv[1],c->db->id);
}