void xreadCommand(client *c) {
    long long timeout = -1; /* -1 means, no BLOCK argument given. */
    long long count = 0;
    int streams_count = 0;
    int streams_arg = 0;
    int noack = 0;          /* True if NOACK option was specified. */
    streamID static_ids[STREAMID_STATIC_VECTOR_LEN];
    streamID *ids = static_ids;
    streamCG **groups = NULL;
    int xreadgroup = sdslen(c->argv[0]->ptr) == 10; /* XREAD or XREADGROUP? */
    robj *groupname = NULL;
    robj *consumername = NULL;

    /* Parse arguments. */
    for (int i = 1; i < c->argc; i++) {
        int moreargs = c->argc-i-1;
        char *o = c->argv[i]->ptr;
        if (!strcasecmp(o,"BLOCK") && moreargs) {
            if (c->flags & CLIENT_SCRIPT) {
                /*
                 * Although the CLIENT_DENY_BLOCKING flag should protect from blocking the client
                 * on Lua/MULTI/RM_Call we want special treatment for Lua to keep backward compatibility.
                 * There is no sense to use BLOCK option within Lua. */
                addReplyErrorFormat(c, "%s command is not allowed with BLOCK option from scripts", (char *)c->argv[0]->ptr);
                return;
            }
            i++;
            if (getTimeoutFromObjectOrReply(c,c->argv[i],&timeout,
                UNIT_MILLISECONDS) != C_OK) return;
        } else if (!strcasecmp(o,"COUNT") && moreargs) {
            i++;
            if (getLongLongFromObjectOrReply(c,c->argv[i],&count,NULL) != C_OK)
                return;
            if (count < 0) count = 0;
        } else if (!strcasecmp(o,"STREAMS") && moreargs) {
            streams_arg = i+1;
            streams_count = (c->argc-streams_arg);
            if ((streams_count % 2) != 0) {
                addReplyError(c,"Unbalanced XREAD list of streams: "
                                "for each stream key an ID or '$' must be "
                                "specified.");
                return;
            }
            streams_count /= 2; /* We have two arguments for each stream. */
            break;
        } else if (!strcasecmp(o,"GROUP") && moreargs >= 2) {
            if (!xreadgroup) {
                addReplyError(c,"The GROUP option is only supported by "
                                "XREADGROUP. You called XREAD instead.");
                return;
            }
            groupname = c->argv[i+1];
            consumername = c->argv[i+2];
            i += 2;
        } else if (!strcasecmp(o,"NOACK")) {
            if (!xreadgroup) {
                addReplyError(c,"The NOACK option is only supported by "
                                "XREADGROUP. You called XREAD instead.");
                return;
            }
            noack = 1;
        } else {
            addReplyErrorObject(c,shared.syntaxerr);
            return;
        }
    }

    /* STREAMS option is mandatory. */
    if (streams_arg == 0) {
        addReplyErrorObject(c,shared.syntaxerr);
        return;
    }

    /* If the user specified XREADGROUP then it must also
     * provide the GROUP option. */
    if (xreadgroup && groupname == NULL) {
        addReplyError(c,"Missing GROUP option for XREADGROUP");
        return;
    }

    /* Parse the IDs and resolve the group name. */
    if (streams_count > STREAMID_STATIC_VECTOR_LEN)
        ids = zmalloc(sizeof(streamID)*streams_count);
    if (groupname) groups = zmalloc(sizeof(streamCG*)*streams_count);

    for (int i = streams_arg + streams_count; i < c->argc; i++) {
        /* Specifying "$" as last-known-id means that the client wants to be
         * served with just the messages that will arrive into the stream
         * starting from now. */
        int id_idx = i - streams_arg - streams_count;
        robj *key = c->argv[i-streams_count];
        robj *o = lookupKeyRead(c->db,key);
        if (checkType(c,o,OBJ_STREAM)) goto cleanup;
        streamCG *group = NULL;

        /* If a group was specified, than we need to be sure that the
         * key and group actually exist. */
        if (groupname) {
            if (o == NULL ||
                (group = streamLookupCG(o->ptr,groupname->ptr)) == NULL)
            {
                addReplyErrorFormat(c, "-NOGROUP No such key '%s' or consumer "
                                       "group '%s' in XREADGROUP with GROUP "
                                       "option",
                                    (char*)key->ptr,(char*)groupname->ptr);
                goto cleanup;
            }
            groups[id_idx] = group;
        }

        if (strcmp(c->argv[i]->ptr,"$") == 0) {
            if (xreadgroup) {
                addReplyError(c,"The $ ID is meaningless in the context of "
                                "XREADGROUP: you want to read the history of "
                                "this consumer by specifying a proper ID, or "
                                "use the > ID to get new messages. The $ ID would "
                                "just return an empty result set.");
                goto cleanup;
            }
            if (o) {
                stream *s = o->ptr;
                ids[id_idx] = s->last_id;
            } else {
                ids[id_idx].ms = 0;
                ids[id_idx].seq = 0;
            }
            continue;
        } else if (strcmp(c->argv[i]->ptr,">") == 0) {
            if (!xreadgroup) {
                addReplyError(c,"The > ID can be specified only when calling "
                                "XREADGROUP using the GROUP <group> "
                                "<consumer> option.");
                goto cleanup;
            }
            /* We use just the maximum ID to signal this is a ">" ID, anyway
             * the code handling the blocking clients will have to update the
             * ID later in order to match the changing consumer group last ID. */
            ids[id_idx].ms = UINT64_MAX;
            ids[id_idx].seq = UINT64_MAX;
            continue;
        }
        if (streamParseStrictIDOrReply(c,c->argv[i],ids+id_idx,0,NULL) != C_OK)
            goto cleanup;
    }

    /* Try to serve the client synchronously. */
    size_t arraylen = 0;
    void *arraylen_ptr = NULL;
    for (int i = 0; i < streams_count; i++) {
        robj *o = lookupKeyRead(c->db,c->argv[streams_arg+i]);
        if (o == NULL) continue;
        stream *s = o->ptr;
        streamID *gt = ids+i; /* ID must be greater than this. */
        int serve_synchronously = 0;
        int serve_history = 0; /* True for XREADGROUP with ID != ">". */

        /* Check if there are the conditions to serve the client
         * synchronously. */
        if (groups) {
            /* If the consumer is blocked on a group, we always serve it
             * synchronously (serving its local history) if the ID specified
             * was not the special ">" ID. */
            if (gt->ms != UINT64_MAX ||
                gt->seq != UINT64_MAX)
            {
                serve_synchronously = 1;
                serve_history = 1;
            } else if (s->length) {
                /* We also want to serve a consumer in a consumer group
                 * synchronously in case the group top item delivered is smaller
                 * than what the stream has inside. */
                streamID maxid, *last = &groups[i]->last_id;
                streamLastValidID(s, &maxid);
                if (streamCompareID(&maxid, last) > 0) {
                    serve_synchronously = 1;
                    *gt = *last;
                }
            }
        } else if (s->length) {
            /* For consumers without a group, we serve synchronously if we can
             * actually provide at least one item from the stream. */
            streamID maxid;
            streamLastValidID(s, &maxid);
            if (streamCompareID(&maxid, gt) > 0) {
                serve_synchronously = 1;
            }
        }

        if (serve_synchronously) {
            arraylen++;
            if (arraylen == 1) arraylen_ptr = addReplyDeferredLen(c);
            /* streamReplyWithRange() handles the 'start' ID as inclusive,
             * so start from the next ID, since we want only messages with
             * IDs greater than start. */
            streamID start = *gt;
            streamIncrID(&start);

            /* Emit the two elements sub-array consisting of the name
             * of the stream and the data we extracted from it. */
            if (c->resp == 2) addReplyArrayLen(c,2);
            addReplyBulk(c,c->argv[streams_arg+i]);
            streamConsumer *consumer = NULL;
            streamPropInfo spi = {c->argv[i+streams_arg],groupname};
            if (groups) {
                consumer = streamLookupConsumer(groups[i],consumername->ptr,SLC_DEFAULT);
                if (consumer == NULL) {
                    consumer = streamCreateConsumer(groups[i],consumername->ptr,
                                                    c->argv[streams_arg+i],
                                                    c->db->id,SCC_DEFAULT);
                    if (noack)
                        streamPropagateConsumerCreation(c,spi.keyname,
                                                        spi.groupname,
                                                        consumer->name);
                }
            }
            int flags = 0;
            if (noack) flags |= STREAM_RWR_NOACK;
            if (serve_history) flags |= STREAM_RWR_HISTORY;
            streamReplyWithRange(c,s,&start,NULL,count,0,
                                 groups ? groups[i] : NULL,
                                 consumer, flags, &spi);
            if (groups) server.dirty++;
        }
    }

     /* We replied synchronously! Set the top array len and return to caller. */
    if (arraylen) {
        if (c->resp == 2)
            setDeferredArrayLen(c,arraylen_ptr,arraylen);
        else
            setDeferredMapLen(c,arraylen_ptr,arraylen);
        goto cleanup;
    }

    /* Block if needed. */
    if (timeout != -1) {
        /* If we are not allowed to block the client, the only thing
         * we can do is treating it as a timeout (even with timeout 0). */
        if (c->flags & CLIENT_DENY_BLOCKING) {
            addReplyNullArray(c);
            goto cleanup;
        }
        blockForKeys(c, BLOCKED_STREAM, c->argv+streams_arg, streams_count,
                     -1, timeout, NULL, NULL, ids);
        /* If no COUNT is given and we block, set a relatively small count:
         * in case the ID provided is too low, we do not want the server to
         * block just to serve this client a huge stream of messages. */
        c->bpop.xread_count = count ? count : XREAD_BLOCKED_DEFAULT_COUNT;

        /* If this is a XREADGROUP + GROUP we need to remember for which
         * group and consumer name we are blocking, so later when one of the
         * keys receive more data, we can call streamReplyWithRange() passing
         * the right arguments. */
        if (groupname) {
            incrRefCount(groupname);
            incrRefCount(consumername);
            c->bpop.xread_group = groupname;
            c->bpop.xread_consumer = consumername;
            c->bpop.xread_group_noack = noack;
        } else {
            c->bpop.xread_group = NULL;
            c->bpop.xread_consumer = NULL;
        }
        goto cleanup;
    }

    /* No BLOCK option, nor any stream we can serve. Reply as with a
     * timeout happened. */
    addReplyNullArray(c);
    /* Continue to cleanup... */

cleanup: /* Cleanup. */

    /* The command is propagated (in the READGROUP form) as a side effect
     * of calling lower level APIs. So stop any implicit propagation. */
    preventCommandPropagation(c);
    if (ids != static_ids) zfree(ids);
    zfree(groups);
}