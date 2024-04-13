streamConsumer *streamCreateConsumer(streamCG *cg, sds name, robj *key, int dbid, int flags) {
    if (cg == NULL) return NULL;
    int notify = !(flags & SCC_NO_NOTIFY);
    int dirty = !(flags & SCC_NO_DIRTIFY);
    streamConsumer *consumer = zmalloc(sizeof(*consumer));
    int success = raxTryInsert(cg->consumers,(unsigned char*)name,
                               sdslen(name),consumer,NULL);
    if (!success) {
        zfree(consumer);
        return NULL;
    }
    consumer->name = sdsdup(name);
    consumer->pel = raxNew();
    consumer->seen_time = mstime();
    if (dirty) server.dirty++;
    if (notify) notifyKeyspaceEvent(NOTIFY_STREAM,"xgroup-createconsumer",key,dbid);
    return consumer;
}