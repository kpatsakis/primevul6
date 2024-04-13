void xdelCommand(client *c) {
    robj *o;

    if ((o = lookupKeyWriteOrReply(c,c->argv[1],shared.czero)) == NULL
        || checkType(c,o,OBJ_STREAM)) return;
    stream *s = o->ptr;

    /* We need to sanity check the IDs passed to start. Even if not
     * a big issue, it is not great that the command is only partially
     * executed because at some point an invalid ID is parsed. */
    streamID static_ids[STREAMID_STATIC_VECTOR_LEN];
    streamID *ids = static_ids;
    int id_count = c->argc-2;
    if (id_count > STREAMID_STATIC_VECTOR_LEN)
        ids = zmalloc(sizeof(streamID)*id_count);
    for (int j = 2; j < c->argc; j++) {
        if (streamParseStrictIDOrReply(c,c->argv[j],&ids[j-2],0,NULL) != C_OK) goto cleanup;
    }

    /* Actually apply the command. */
    int deleted = 0;
    int first_entry = 0;
    for (int j = 2; j < c->argc; j++) {
        streamID *id = &ids[j-2];
        if (streamDeleteItem(s,id)) {
            /* We want to know if the first entry in the stream was deleted
             * so we can later set the new one. */
            if (streamCompareID(id,&s->first_id) == 0) {
                first_entry = 1;
            }
            /* Update the stream's maximal tombstone if needed. */
            if (streamCompareID(id,&s->max_deleted_entry_id) > 0) {
                s->max_deleted_entry_id = *id;
            }
            deleted++;
        };
    }

    /* Update the stream's first ID. */
    if (deleted) {
        if (s->length == 0) {
            s->first_id.ms = 0;
            s->first_id.seq = 0;
        } else if (first_entry) {
            streamGetEdgeID(s,1,1,&s->first_id);
        }
    }

    /* Propagate the write if needed. */
    if (deleted) {
        signalModifiedKey(c,c->db,c->argv[1]);
        notifyKeyspaceEvent(NOTIFY_STREAM,"xdel",c->argv[1],c->db->id);
        server.dirty += deleted;
    }
    addReplyLongLong(c,deleted);
cleanup:
    if (ids != static_ids) zfree(ids);
}