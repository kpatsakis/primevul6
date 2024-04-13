void streamLastValidID(stream *s, streamID *maxid)
{
    streamIterator si;
    streamIteratorStart(&si,s,NULL,NULL,1);
    int64_t numfields;
    if (!streamIteratorGetID(&si,maxid,&numfields) && s->length)
        serverPanic("Corrupt stream, length is %llu, but no max id", (unsigned long long)s->length);
    streamIteratorStop(&si);
}