size_t streamReplyWithRangeFromConsumerPEL(client *c, stream *s, streamID *start, streamID *end, size_t count, streamConsumer *consumer) {
    raxIterator ri;
    unsigned char startkey[sizeof(streamID)];
    unsigned char endkey[sizeof(streamID)];
    streamEncodeID(startkey,start);
    if (end) streamEncodeID(endkey,end);

    size_t arraylen = 0;
    void *arraylen_ptr = addReplyDeferredLen(c);
    raxStart(&ri,consumer->pel);
    raxSeek(&ri,">=",startkey,sizeof(startkey));
    while(raxNext(&ri) && (!count || arraylen < count)) {
        if (end && memcmp(ri.key,end,ri.key_len) > 0) break;
        streamID thisid;
        streamDecodeID(ri.key,&thisid);
        if (streamReplyWithRange(c,s,&thisid,&thisid,1,0,NULL,NULL,
                                 STREAM_RWR_RAWENTRIES,NULL) == 0)
        {
            /* Note that we may have a not acknowledged entry in the PEL
             * about a message that's no longer here because was removed
             * by the user by other means. In that case we signal it emitting
             * the ID but then a NULL entry for the fields. */
            addReplyArrayLen(c,2);
            addReplyStreamID(c,&thisid);
            addReplyNullArray(c);
        } else {
            streamNACK *nack = ri.data;
            nack->delivery_time = mstime();
            nack->delivery_count++;
        }
        arraylen++;
    }
    raxStop(&ri);
    setDeferredArrayLen(c,arraylen_ptr,arraylen);
    return arraylen;
}