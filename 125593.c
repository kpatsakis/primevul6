void streamReplyWithCGLag(client *c, stream *s, streamCG *cg) {
    int valid = 0;
    long long lag = 0;

    if (!s->entries_added) {
        /* The lag of a newly-initialized stream is 0. */
        lag = 0;
        valid = 1;
    } else if (cg->entries_read != SCG_INVALID_ENTRIES_READ && !streamRangeHasTombstones(s,&cg->last_id,NULL)) {
        /* No fragmentation ahead means that the group's logical reads counter
         * is valid for performing the lag calculation. */
        lag = (long long)s->entries_added - cg->entries_read;
        valid = 1;
    } else {
        /* Attempt to retrieve the group's last ID logical read counter. */
        long long entries_read = streamEstimateDistanceFromFirstEverEntry(s,&cg->last_id);
        if (entries_read != SCG_INVALID_ENTRIES_READ) {
            /* A valid counter was obtained. */
            lag = (long long)s->entries_added - entries_read;
            valid = 1;
        }
    }

    if (valid) {
        addReplyLongLong(c,lag);
    } else {
        addReplyNull(c);
    }
}