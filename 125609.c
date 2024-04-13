void xackCommand(client *c) {
    streamCG *group = NULL;
    robj *o = lookupKeyRead(c->db,c->argv[1]);
    if (o) {
        if (checkType(c,o,OBJ_STREAM)) return; /* Type error. */
        group = streamLookupCG(o->ptr,c->argv[2]->ptr);
    }

    /* No key or group? Nothing to ack. */
    if (o == NULL || group == NULL) {
        addReply(c,shared.czero);
        return;
    }

    /* Start parsing the IDs, so that we abort ASAP if there is a syntax
     * error: the return value of this command cannot be an error in case
     * the client successfully acknowledged some messages, so it should be
     * executed in a "all or nothing" fashion. */
    streamID static_ids[STREAMID_STATIC_VECTOR_LEN];
    streamID *ids = static_ids;
    int id_count = c->argc-3;
    if (id_count > STREAMID_STATIC_VECTOR_LEN)
        ids = zmalloc(sizeof(streamID)*id_count);
    for (int j = 3; j < c->argc; j++) {
        if (streamParseStrictIDOrReply(c,c->argv[j],&ids[j-3],0,NULL) != C_OK) goto cleanup;
    }

    int acknowledged = 0;
    for (int j = 3; j < c->argc; j++) {
        unsigned char buf[sizeof(streamID)];
        streamEncodeID(buf,&ids[j-3]);

        /* Lookup the ID in the group PEL: it will have a reference to the
         * NACK structure that will have a reference to the consumer, so that
         * we are able to remove the entry from both PELs. */
        streamNACK *nack = raxFind(group->pel,buf,sizeof(buf));
        if (nack != raxNotFound) {
            raxRemove(group->pel,buf,sizeof(buf),NULL);
            raxRemove(nack->consumer->pel,buf,sizeof(buf),NULL);
            streamFreeNACK(nack);
            acknowledged++;
            server.dirty++;
        }
    }
    addReplyLongLong(c,acknowledged);
cleanup:
    if (ids != static_ids) zfree(ids);
}