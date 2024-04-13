void streamDelConsumer(streamCG *cg, streamConsumer *consumer) {
    /* Iterate all the consumer pending messages, deleting every corresponding
     * entry from the global entry. */
    raxIterator ri;
    raxStart(&ri,consumer->pel);
    raxSeek(&ri,"^",NULL,0);
    while(raxNext(&ri)) {
        streamNACK *nack = ri.data;
        raxRemove(cg->pel,ri.key,ri.key_len,NULL);
        streamFreeNACK(nack);
    }
    raxStop(&ri);

    /* Deallocate the consumer. */
    raxRemove(cg->consumers,(unsigned char*)consumer->name,
              sdslen(consumer->name),NULL);
    streamFreeConsumer(consumer);
}