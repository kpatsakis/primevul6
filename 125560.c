size_t streamReplyWithRange(client *c, stream *s, streamID *start, streamID *end, size_t count, int rev, streamCG *group, streamConsumer *consumer, int flags, streamPropInfo *spi) {
    void *arraylen_ptr = NULL;
    size_t arraylen = 0;
    streamIterator si;
    int64_t numfields;
    streamID id;
    int propagate_last_id = 0;
    int noack = flags & STREAM_RWR_NOACK;

    /* If the client is asking for some history, we serve it using a
     * different function, so that we return entries *solely* from its
     * own PEL. This ensures each consumer will always and only see
     * the history of messages delivered to it and not yet confirmed
     * as delivered. */
    if (group && (flags & STREAM_RWR_HISTORY)) {
        return streamReplyWithRangeFromConsumerPEL(c,s,start,end,count,
                                                   consumer);
    }

    if (!(flags & STREAM_RWR_RAWENTRIES))
        arraylen_ptr = addReplyDeferredLen(c);
    streamIteratorStart(&si,s,start,end,rev);
    while(streamIteratorGetID(&si,&id,&numfields)) {
        /* Update the group last_id if needed. */
        if (group && streamCompareID(&id,&group->last_id) > 0) {
            if (group->entries_read != SCG_INVALID_ENTRIES_READ && !streamRangeHasTombstones(s,&id,NULL)) {
                /* A valid counter and no future tombstones mean we can 
                 * increment the read counter to keep tracking the group's
                 * progress. */
                group->entries_read++;
            } else if (s->entries_added) {
                /* The group's counter may be invalid, so we try to obtain it. */
                group->entries_read = streamEstimateDistanceFromFirstEverEntry(s,&id);
            }
            group->last_id = id;
            /* Group last ID should be propagated only if NOACK was
             * specified, otherwise the last id will be included
             * in the propagation of XCLAIM itself. */
            if (noack) propagate_last_id = 1;
        }

        /* Emit a two elements array for each item. The first is
         * the ID, the second is an array of field-value pairs. */
        addReplyArrayLen(c,2);
        addReplyStreamID(c,&id);

        addReplyArrayLen(c,numfields*2);

        /* Emit the field-value pairs. */
        while(numfields--) {
            unsigned char *key, *value;
            int64_t key_len, value_len;
            streamIteratorGetField(&si,&key,&value,&key_len,&value_len);
            addReplyBulkCBuffer(c,key,key_len);
            addReplyBulkCBuffer(c,value,value_len);
        }

        /* If a group is passed, we need to create an entry in the
         * PEL (pending entries list) of this group *and* this consumer.
         *
         * Note that we cannot be sure about the fact the message is not
         * already owned by another consumer, because the admin is able
         * to change the consumer group last delivered ID using the
         * XGROUP SETID command. So if we find that there is already
         * a NACK for the entry, we need to associate it to the new
         * consumer. */
        if (group && !noack) {
            unsigned char buf[sizeof(streamID)];
            streamEncodeID(buf,&id);

            /* Try to add a new NACK. Most of the time this will work and
             * will not require extra lookups. We'll fix the problem later
             * if we find that there is already a entry for this ID. */
            streamNACK *nack = streamCreateNACK(consumer);
            int group_inserted =
                raxTryInsert(group->pel,buf,sizeof(buf),nack,NULL);
            int consumer_inserted =
                raxTryInsert(consumer->pel,buf,sizeof(buf),nack,NULL);

            /* Now we can check if the entry was already busy, and
             * in that case reassign the entry to the new consumer,
             * or update it if the consumer is the same as before. */
            if (group_inserted == 0) {
                streamFreeNACK(nack);
                nack = raxFind(group->pel,buf,sizeof(buf));
                serverAssert(nack != raxNotFound);
                raxRemove(nack->consumer->pel,buf,sizeof(buf),NULL);
                /* Update the consumer and NACK metadata. */
                nack->consumer = consumer;
                nack->delivery_time = mstime();
                nack->delivery_count = 1;
                /* Add the entry in the new consumer local PEL. */
                raxInsert(consumer->pel,buf,sizeof(buf),nack,NULL);
            } else if (group_inserted == 1 && consumer_inserted == 0) {
                serverPanic("NACK half-created. Should not be possible.");
            }

            /* Propagate as XCLAIM. */
            if (spi) {
                robj *idarg = createObjectFromStreamID(&id);
                streamPropagateXCLAIM(c,spi->keyname,group,spi->groupname,idarg,nack);
                decrRefCount(idarg);
            }
        }

        arraylen++;
        if (count && count == arraylen) break;
    }

    if (spi && propagate_last_id)
        streamPropagateGroupID(c,spi->keyname,group,spi->groupname);

    streamIteratorStop(&si);
    if (arraylen_ptr) setDeferredArrayLen(c,arraylen_ptr,arraylen);
    return arraylen;
}