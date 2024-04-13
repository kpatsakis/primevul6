void streamIteratorStart(streamIterator *si, stream *s, streamID *start, streamID *end, int rev) {
    /* Initialize the iterator and translates the iteration start/stop
     * elements into a 128 big big-endian number. */
    if (start) {
        streamEncodeID(si->start_key,start);
    } else {
        si->start_key[0] = 0;
        si->start_key[1] = 0;
    }

    if (end) {
        streamEncodeID(si->end_key,end);
    } else {
        si->end_key[0] = UINT64_MAX;
        si->end_key[1] = UINT64_MAX;
    }

    /* Seek the correct node in the radix tree. */
    raxStart(&si->ri,s->rax);
    if (!rev) {
        if (start && (start->ms || start->seq)) {
            raxSeek(&si->ri,"<=",(unsigned char*)si->start_key,
                    sizeof(si->start_key));
            if (raxEOF(&si->ri)) raxSeek(&si->ri,"^",NULL,0);
        } else {
            raxSeek(&si->ri,"^",NULL,0);
        }
    } else {
        if (end && (end->ms || end->seq)) {
            raxSeek(&si->ri,"<=",(unsigned char*)si->end_key,
                    sizeof(si->end_key));
            if (raxEOF(&si->ri)) raxSeek(&si->ri,"$",NULL,0);
        } else {
            raxSeek(&si->ri,"$",NULL,0);
        }
    }
    si->stream = s;
    si->lp = NULL;     /* There is no current listpack right now. */
    si->lp_ele = NULL; /* Current listpack cursor. */
    si->rev = rev;     /* Direction, if non-zero reversed, from end to start. */
    si->skip_tombstones = 1;    /* By default tombstones aren't emitted. */
}