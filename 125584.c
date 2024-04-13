void xclaimCommand(client *c) {
    streamCG *group = NULL;
    robj *o = lookupKeyRead(c->db,c->argv[1]);
    long long minidle; /* Minimum idle time argument. */
    long long retrycount = -1;   /* -1 means RETRYCOUNT option not given. */
    mstime_t deliverytime = -1;  /* -1 means IDLE/TIME options not given. */
    int force = 0;
    int justid = 0;

    if (o) {
        if (checkType(c,o,OBJ_STREAM)) return; /* Type error. */
        group = streamLookupCG(o->ptr,c->argv[2]->ptr);
    }

    /* No key or group? Send an error given that the group creation
     * is mandatory. */
    if (o == NULL || group == NULL) {
        addReplyErrorFormat(c,"-NOGROUP No such key '%s' or "
                              "consumer group '%s'", (char*)c->argv[1]->ptr,
                              (char*)c->argv[2]->ptr);
        return;
    }

    if (getLongLongFromObjectOrReply(c,c->argv[4],&minidle,
        "Invalid min-idle-time argument for XCLAIM")
        != C_OK) return;
    if (minidle < 0) minidle = 0;

    /* Start parsing the IDs, so that we abort ASAP if there is a syntax
     * error: the return value of this command cannot be an error in case
     * the client successfully claimed some message, so it should be
     * executed in a "all or nothing" fashion. */
    int j;
    streamID static_ids[STREAMID_STATIC_VECTOR_LEN];
    streamID *ids = static_ids;
    int id_count = c->argc-5;
    if (id_count > STREAMID_STATIC_VECTOR_LEN)
        ids = zmalloc(sizeof(streamID)*id_count);
    for (j = 5; j < c->argc; j++) {
        if (streamParseStrictIDOrReply(NULL,c->argv[j],&ids[j-5],0,NULL) != C_OK) break;
    }
    int last_id_arg = j-1; /* Next time we iterate the IDs we now the range. */

    /* If we stopped because some IDs cannot be parsed, perhaps they
     * are trailing options. */
    mstime_t now = mstime();
    streamID last_id = {0,0};
    int propagate_last_id = 0;
    for (; j < c->argc; j++) {
        int moreargs = (c->argc-1) - j; /* Number of additional arguments. */
        char *opt = c->argv[j]->ptr;
        if (!strcasecmp(opt,"FORCE")) {
            force = 1;
        } else if (!strcasecmp(opt,"JUSTID")) {
            justid = 1;
        } else if (!strcasecmp(opt,"IDLE") && moreargs) {
            j++;
            if (getLongLongFromObjectOrReply(c,c->argv[j],&deliverytime,
                "Invalid IDLE option argument for XCLAIM")
                != C_OK) goto cleanup;
            deliverytime = now - deliverytime;
        } else if (!strcasecmp(opt,"TIME") && moreargs) {
            j++;
            if (getLongLongFromObjectOrReply(c,c->argv[j],&deliverytime,
                "Invalid TIME option argument for XCLAIM")
                != C_OK) goto cleanup;
        } else if (!strcasecmp(opt,"RETRYCOUNT") && moreargs) {
            j++;
            if (getLongLongFromObjectOrReply(c,c->argv[j],&retrycount,
                "Invalid RETRYCOUNT option argument for XCLAIM")
                != C_OK) goto cleanup;
        } else if (!strcasecmp(opt,"LASTID") && moreargs) {
            j++;
            if (streamParseStrictIDOrReply(c,c->argv[j],&last_id,0,NULL) != C_OK) goto cleanup;
        } else {
            addReplyErrorFormat(c,"Unrecognized XCLAIM option '%s'",opt);
            goto cleanup;
        }
    }

    if (streamCompareID(&last_id,&group->last_id) > 0) {
        group->last_id = last_id;
        propagate_last_id = 1;
    }

    if (deliverytime != -1) {
        /* If a delivery time was passed, either with IDLE or TIME, we
         * do some sanity check on it, and set the deliverytime to now
         * (which is a sane choice usually) if the value is bogus.
         * To raise an error here is not wise because clients may compute
         * the idle time doing some math starting from their local time,
         * and this is not a good excuse to fail in case, for instance,
         * the computer time is a bit in the future from our POV. */
        if (deliverytime < 0 || deliverytime > now) deliverytime = now;
    } else {
        /* If no IDLE/TIME option was passed, we want the last delivery
         * time to be now, so that the idle time of the message will be
         * zero. */
        deliverytime = now;
    }

    /* Do the actual claiming. */
    streamConsumer *consumer = NULL;
    void *arraylenptr = addReplyDeferredLen(c);
    size_t arraylen = 0;
    sds name = c->argv[3]->ptr;
    for (int j = 5; j <= last_id_arg; j++) {
        streamID id = ids[j-5];
        unsigned char buf[sizeof(streamID)];
        streamEncodeID(buf,&id);

        /* Lookup the ID in the group PEL. */
        streamNACK *nack = raxFind(group->pel,buf,sizeof(buf));

        /* Item must exist for us to transfer it to another consumer. */
        if (!streamEntryExists(o->ptr,&id)) {
            /* Clear this entry from the PEL, it no longer exists */
            if (nack != raxNotFound) {
                /* Propagate this change (we are going to delete the NACK). */
                streamPropagateXCLAIM(c,c->argv[1],group,c->argv[2],c->argv[j],nack);
                propagate_last_id = 0; /* Will be propagated by XCLAIM itself. */
                server.dirty++;
                /* Release the NACK */
                raxRemove(group->pel,buf,sizeof(buf),NULL);
                raxRemove(nack->consumer->pel,buf,sizeof(buf),NULL);
                streamFreeNACK(nack);
            }
            continue;
        }

        /* If FORCE is passed, let's check if at least the entry
         * exists in the Stream. In such case, we'll create a new
         * entry in the PEL from scratch, so that XCLAIM can also
         * be used to create entries in the PEL. Useful for AOF
         * and replication of consumer groups. */
        if (force && nack == raxNotFound) {
            /* Create the NACK. */
            nack = streamCreateNACK(NULL);
            raxInsert(group->pel,buf,sizeof(buf),nack,NULL);
        }

        if (nack != raxNotFound) {
            /* We need to check if the minimum idle time requested
             * by the caller is satisfied by this entry.
             *
             * Note that the nack could be created by FORCE, in this
             * case there was no pre-existing entry and minidle should
             * be ignored, but in that case nack->consumer is NULL. */
            if (nack->consumer && minidle) {
                mstime_t this_idle = now - nack->delivery_time;
                if (this_idle < minidle) continue;
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
                    raxRemove(nack->consumer->pel,buf,sizeof(buf),NULL);
            }
            nack->delivery_time = deliverytime;
            /* Set the delivery attempts counter if given, otherwise
             * autoincrement unless JUSTID option provided */
            if (retrycount >= 0) {
                nack->delivery_count = retrycount;
            } else if (!justid) {
                nack->delivery_count++;
            }
            if (nack->consumer != consumer) {
                /* Add the entry in the new consumer local PEL. */
                raxInsert(consumer->pel,buf,sizeof(buf),nack,NULL);
                nack->consumer = consumer;
            }
            /* Send the reply for this entry. */
            if (justid) {
                addReplyStreamID(c,&id);
            } else {
                serverAssert(streamReplyWithRange(c,o->ptr,&id,&id,1,0,NULL,NULL,STREAM_RWR_RAWENTRIES,NULL) == 1);
            }
            arraylen++;

            /* Propagate this change. */
            streamPropagateXCLAIM(c,c->argv[1],group,c->argv[2],c->argv[j],nack);
            propagate_last_id = 0; /* Will be propagated by XCLAIM itself. */
            server.dirty++;
        }
    }
    if (propagate_last_id) {
        streamPropagateGroupID(c,c->argv[1],group,c->argv[2]);
        server.dirty++;
    }
    setDeferredArrayLen(c,arraylenptr,arraylen);
    preventCommandPropagation(c);
cleanup:
    if (ids != static_ids) zfree(ids);
}