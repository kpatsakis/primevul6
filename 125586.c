void xautoclaimCommand(client *c) {
    streamCG *group = NULL;
    robj *o = lookupKeyRead(c->db,c->argv[1]);
    long long minidle; /* Minimum idle time argument, in milliseconds. */
    long count = 100; /* Maximum entries to claim. */
    streamID startid;
    int startex;
    int justid = 0;

    /* Parse idle/start/end/count arguments ASAP if needed, in order to report
     * syntax errors before any other error. */
    if (getLongLongFromObjectOrReply(c,c->argv[4],&minidle,"Invalid min-idle-time argument for XAUTOCLAIM") != C_OK)
        return;
    if (minidle < 0) minidle = 0;

    if (streamParseIntervalIDOrReply(c,c->argv[5],&startid,&startex,0) != C_OK)
        return;
    if (startex && streamIncrID(&startid) != C_OK) {
        addReplyError(c,"invalid start ID for the interval");
        return;
    }

    int j = 6; /* options start at argv[6] */
    while(j < c->argc) {
        int moreargs = (c->argc-1) - j; /* Number of additional arguments. */
        char *opt = c->argv[j]->ptr;
        if (!strcasecmp(opt,"COUNT") && moreargs) {
            if (getRangeLongFromObjectOrReply(c,c->argv[j+1],1,LONG_MAX,&count,"COUNT must be > 0") != C_OK)
                return;
            j++;
        } else if (!strcasecmp(opt,"JUSTID")) {
            justid = 1;
        } else {
            addReplyErrorObject(c,shared.syntaxerr);
            return;
        }
        j++;
    }

    if (o) {
        if (checkType(c,o,OBJ_STREAM))
            return; /* Type error. */
        group = streamLookupCG(o->ptr,c->argv[2]->ptr);
    }

    /* No key or group? Send an error given that the group creation
     * is mandatory. */
    if (o == NULL || group == NULL) {
        addReplyErrorFormat(c,"-NOGROUP No such key '%s' or consumer group '%s'",
                            (char*)c->argv[1]->ptr,
                            (char*)c->argv[2]->ptr);
        return;
    }

    /* Do the actual claiming. */
    streamConsumer *consumer = NULL;
    long long attempts = count*10;

    addReplyArrayLen(c, 3); /* We add another reply later */
    void *endidptr = addReplyDeferredLen(c); /* reply[0] */
    void *arraylenptr = addReplyDeferredLen(c); /* reply[1] */

    unsigned char startkey[sizeof(streamID)];
    streamEncodeID(startkey,&startid);
    raxIterator ri;
    raxStart(&ri,group->pel);
    raxSeek(&ri,">=",startkey,sizeof(startkey));
    size_t arraylen = 0;
    mstime_t now = mstime();
    sds name = c->argv[3]->ptr;
    streamID *deleted_ids = zmalloc(count * sizeof(streamID));
    int deleted_id_num = 0;
    while (attempts-- && count && raxNext(&ri)) {
        streamNACK *nack = ri.data;

        streamID id;
        streamDecodeID(ri.key, &id);

        /* Item must exist for us to transfer it to another consumer. */
        if (!streamEntryExists(o->ptr,&id)) {
            /* Propagate this change (we are going to delete the NACK). */
            robj *idstr = createObjectFromStreamID(&id);
            streamPropagateXCLAIM(c,c->argv[1],group,c->argv[2],idstr,nack);
            decrRefCount(idstr);
            server.dirty++;
            /* Clear this entry from the PEL, it no longer exists */
            raxRemove(group->pel,ri.key,ri.key_len,NULL);
            raxRemove(nack->consumer->pel,ri.key,ri.key_len,NULL);
            streamFreeNACK(nack);
            /* Remember the ID for later */
            deleted_ids[deleted_id_num++] = id;
            raxSeek(&ri,">=",ri.key,ri.key_len);
            continue;
        }

        if (minidle) {
            mstime_t this_idle = now - nack->delivery_time;
            if (this_idle < minidle)
                continue;
        }

        if (consumer == NULL &&
            (consumer = streamLookupConsumer(group,name,SLC_DEFAULT)) == NULL)
        {
            consumer = streamCreateConsumer(group,name,c->argv[1],c->db->id,SCC_DEFAULT);
        }
        if (nack->consumer != consumer) {
            /* Remove the entry from the old consumer.
             * Note that nack->consumer is NULL if we created the
             * NACK above because of the FORCE option. */
            if (nack->consumer)
                raxRemove(nack->consumer->pel,ri.key,ri.key_len,NULL);
        }

        /* Update the consumer and idle time. */
        nack->delivery_time = now;
        /* Increment the delivery attempts counter unless JUSTID option provided */
        if (!justid)
            nack->delivery_count++;

        if (nack->consumer != consumer) {
            /* Add the entry in the new consumer local PEL. */
            raxInsert(consumer->pel,ri.key,ri.key_len,nack,NULL);
            nack->consumer = consumer;
        }

        /* Send the reply for this entry. */
        if (justid) {
            addReplyStreamID(c,&id);
        } else {
            serverAssert(streamReplyWithRange(c,o->ptr,&id,&id,1,0,NULL,NULL,STREAM_RWR_RAWENTRIES,NULL) == 1);
        }
        arraylen++;
        count--;

        /* Propagate this change. */
        robj *idstr = createObjectFromStreamID(&id);
        streamPropagateXCLAIM(c,c->argv[1],group,c->argv[2],idstr,nack);
        decrRefCount(idstr);
        server.dirty++;
    }

    /* We need to return the next entry as a cursor for the next XAUTOCLAIM call */
    raxNext(&ri);

    streamID endid;
    if (raxEOF(&ri)) {
        endid.ms = endid.seq = 0;
    } else {
        streamDecodeID(ri.key, &endid);
    }
    raxStop(&ri);

    setDeferredArrayLen(c,arraylenptr,arraylen);
    setDeferredReplyStreamID(c,endidptr,&endid);

    addReplyArrayLen(c, deleted_id_num); /* reply[2] */
    for (int i = 0; i < deleted_id_num; i++) {
        addReplyStreamID(c, &deleted_ids[i]);
    }
    zfree(deleted_ids);

    preventCommandPropagation(c);
}