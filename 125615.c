streamConsumer *streamLookupConsumer(streamCG *cg, sds name, int flags) {
    if (cg == NULL) return NULL;
    int refresh = !(flags & SLC_NO_REFRESH);
    streamConsumer *consumer = raxFind(cg->consumers,(unsigned char*)name,
                                       sdslen(name));
    if (consumer == raxNotFound) return NULL;
    if (refresh) consumer->seen_time = mstime();
    return consumer;
}