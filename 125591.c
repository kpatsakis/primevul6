int streamEntryExists(stream *s, streamID *id) {
    streamIterator si;
    streamIteratorStart(&si,s,id,id,0);
    streamID myid;
    int64_t numfields;
    int found = streamIteratorGetID(&si,&myid,&numfields);
    streamIteratorStop(&si);
    if (!found)
        return 0;
    serverAssert(streamCompareID(id,&myid) == 0);
    return 1;
}