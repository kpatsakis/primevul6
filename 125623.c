void xinfoReplyWithStreamInfo(client *c, stream *s) {
    int full = 1;
    long long count = 10; /* Default COUNT is 10 so we don't block the server */
    robj **optv = c->argv + 3; /* Options start after XINFO STREAM <key> */
    int optc = c->argc - 3;

    /* Parse options. */
    if (optc == 0) {
        full = 0;
    } else {
        /* Valid options are [FULL] or [FULL COUNT <count>] */
        if (optc != 1 && optc != 3) {
            addReplySubcommandSyntaxError(c);
            return;
        }

        /* First option must be "FULL" */
        if (strcasecmp(optv[0]->ptr,"full")) {
            addReplySubcommandSyntaxError(c);
            return;
        }

        if (optc == 3) {
            /* First option must be "FULL" */
            if (strcasecmp(optv[1]->ptr,"count")) {
                addReplySubcommandSyntaxError(c);
                return;
            }
            if (getLongLongFromObjectOrReply(c,optv[2],&count,NULL) == C_ERR)
                return;
            if (count < 0) count = 10;
        }
    }

    addReplyMapLen(c,full ? 9 : 10);
    addReplyBulkCString(c,"length");
    addReplyLongLong(c,s->length);
    addReplyBulkCString(c,"radix-tree-keys");
    addReplyLongLong(c,raxSize(s->rax));
    addReplyBulkCString(c,"radix-tree-nodes");
    addReplyLongLong(c,s->rax->numnodes);
    addReplyBulkCString(c,"last-generated-id");
    addReplyStreamID(c,&s->last_id);
    addReplyBulkCString(c,"max-deleted-entry-id");
    addReplyStreamID(c,&s->max_deleted_entry_id);
    addReplyBulkCString(c,"entries-added");
    addReplyLongLong(c,s->entries_added);
    addReplyBulkCString(c,"recorded-first-entry-id");
    addReplyStreamID(c,&s->first_id);

    if (!full) {
        /* XINFO STREAM <key> */

        addReplyBulkCString(c,"groups");
        addReplyLongLong(c,s->cgroups ? raxSize(s->cgroups) : 0);

        /* To emit the first/last entry we use streamReplyWithRange(). */
        int emitted;
        streamID start, end;
        start.ms = start.seq = 0;
        end.ms = end.seq = UINT64_MAX;
        addReplyBulkCString(c,"first-entry");
        emitted = streamReplyWithRange(c,s,&start,&end,1,0,NULL,NULL,
                                       STREAM_RWR_RAWENTRIES,NULL);
        if (!emitted) addReplyNull(c);
        addReplyBulkCString(c,"last-entry");
        emitted = streamReplyWithRange(c,s,&start,&end,1,1,NULL,NULL,
                                       STREAM_RWR_RAWENTRIES,NULL);
        if (!emitted) addReplyNull(c);
    } else {
        /* XINFO STREAM <key> FULL [COUNT <count>] */

        /* Stream entries */
        addReplyBulkCString(c,"entries");
        streamReplyWithRange(c,s,NULL,NULL,count,0,NULL,NULL,0,NULL);

        /* Consumer groups */
        addReplyBulkCString(c,"groups");
        if (s->cgroups == NULL) {
            addReplyArrayLen(c,0);
        } else {
            addReplyArrayLen(c,raxSize(s->cgroups));
            raxIterator ri_cgroups;
            raxStart(&ri_cgroups,s->cgroups);
            raxSeek(&ri_cgroups,"^",NULL,0);
            while(raxNext(&ri_cgroups)) {
                streamCG *cg = ri_cgroups.data;
                addReplyMapLen(c,7);

                /* Name */
                addReplyBulkCString(c,"name");
                addReplyBulkCBuffer(c,ri_cgroups.key,ri_cgroups.key_len);

                /* Last delivered ID */
                addReplyBulkCString(c,"last-delivered-id");
                addReplyStreamID(c,&cg->last_id);

                /* Read counter of the last delivered ID */
                addReplyBulkCString(c,"entries-read");
                if (cg->entries_read != SCG_INVALID_ENTRIES_READ) {
                    addReplyLongLong(c,cg->entries_read);
                } else {
                    addReplyNull(c);
                }

                /* Group lag */
                addReplyBulkCString(c,"lag");
                streamReplyWithCGLag(c,s,cg);

                /* Group PEL count */
                addReplyBulkCString(c,"pel-count");
                addReplyLongLong(c,raxSize(cg->pel));

                /* Group PEL */
                addReplyBulkCString(c,"pending");
                long long arraylen_cg_pel = 0;
                void *arrayptr_cg_pel = addReplyDeferredLen(c);
                raxIterator ri_cg_pel;
                raxStart(&ri_cg_pel,cg->pel);
                raxSeek(&ri_cg_pel,"^",NULL,0);
                while(raxNext(&ri_cg_pel) && (!count || arraylen_cg_pel < count)) {
                    streamNACK *nack = ri_cg_pel.data;
                    addReplyArrayLen(c,4);

                    /* Entry ID. */
                    streamID id;
                    streamDecodeID(ri_cg_pel.key,&id);
                    addReplyStreamID(c,&id);

                    /* Consumer name. */
                    serverAssert(nack->consumer); /* assertion for valgrind (avoid NPD) */
                    addReplyBulkCBuffer(c,nack->consumer->name,
                                        sdslen(nack->consumer->name));

                    /* Last delivery. */
                    addReplyLongLong(c,nack->delivery_time);

                    /* Number of deliveries. */
                    addReplyLongLong(c,nack->delivery_count);

                    arraylen_cg_pel++;
                }
                setDeferredArrayLen(c,arrayptr_cg_pel,arraylen_cg_pel);
                raxStop(&ri_cg_pel);

                /* Consumers */
                addReplyBulkCString(c,"consumers");
                addReplyArrayLen(c,raxSize(cg->consumers));
                raxIterator ri_consumers;
                raxStart(&ri_consumers,cg->consumers);
                raxSeek(&ri_consumers,"^",NULL,0);
                while(raxNext(&ri_consumers)) {
                    streamConsumer *consumer = ri_consumers.data;
                    addReplyMapLen(c,4);

                    /* Consumer name */
                    addReplyBulkCString(c,"name");
                    addReplyBulkCBuffer(c,consumer->name,sdslen(consumer->name));

                    /* Seen-time */
                    addReplyBulkCString(c,"seen-time");
                    addReplyLongLong(c,consumer->seen_time);

                    /* Consumer PEL count */
                    addReplyBulkCString(c,"pel-count");
                    addReplyLongLong(c,raxSize(consumer->pel));

                    /* Consumer PEL */
                    addReplyBulkCString(c,"pending");
                    long long arraylen_cpel = 0;
                    void *arrayptr_cpel = addReplyDeferredLen(c);
                    raxIterator ri_cpel;
                    raxStart(&ri_cpel,consumer->pel);
                    raxSeek(&ri_cpel,"^",NULL,0);
                    while(raxNext(&ri_cpel) && (!count || arraylen_cpel < count)) {
                        streamNACK *nack = ri_cpel.data;
                        addReplyArrayLen(c,3);

                        /* Entry ID. */
                        streamID id;
                        streamDecodeID(ri_cpel.key,&id);
                        addReplyStreamID(c,&id);

                        /* Last delivery. */
                        addReplyLongLong(c,nack->delivery_time);

                        /* Number of deliveries. */
                        addReplyLongLong(c,nack->delivery_count);

                        arraylen_cpel++;
                    }
                    setDeferredArrayLen(c,arrayptr_cpel,arraylen_cpel);
                    raxStop(&ri_cpel);
                }
                raxStop(&ri_consumers);
            }
            raxStop(&ri_cgroups);
        }
    }
}