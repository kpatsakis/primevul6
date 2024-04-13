int64_t streamTrim(stream *s, streamAddTrimArgs *args) {
    size_t maxlen = args->maxlen;
    streamID *id = &args->minid;
    int approx = args->approx_trim;
    int64_t limit = args->limit;
    int trim_strategy = args->trim_strategy;

    if (trim_strategy == TRIM_STRATEGY_NONE)
        return 0;

    raxIterator ri;
    raxStart(&ri,s->rax);
    raxSeek(&ri,"^",NULL,0);

    int64_t deleted = 0;
    while (raxNext(&ri)) {
        if (trim_strategy == TRIM_STRATEGY_MAXLEN && s->length <= maxlen)
            break;

        unsigned char *lp = ri.data, *p = lpFirst(lp);
        int64_t entries = lpGetInteger(p);

        /* Check if we exceeded the amount of work we could do */
        if (limit && (deleted + entries) > limit)
            break;

        /* Check if we can remove the whole node. */
        int remove_node;
        streamID master_id = {0}; /* For MINID */
        if (trim_strategy == TRIM_STRATEGY_MAXLEN) {
            remove_node = s->length - entries >= maxlen;
        } else {
            /* Read the master ID from the radix tree key. */
            streamDecodeID(ri.key, &master_id);

            /* Read last ID. */
            streamID last_id;
            lpGetEdgeStreamID(lp, 0, &master_id, &last_id);

            /* We can remove the entire node id its last ID < 'id' */
            remove_node = streamCompareID(&last_id, id) < 0;
        }

        if (remove_node) {
            lpFree(lp);
            raxRemove(s->rax,ri.key,ri.key_len,NULL);
            raxSeek(&ri,">=",ri.key,ri.key_len);
            s->length -= entries;
            deleted += entries;
            continue;
        }

        /* If we cannot remove a whole element, and approx is true,
         * stop here. */
        if (approx) break;

        /* Now we have to trim entries from within 'lp' */
        int64_t deleted_from_lp = 0;

        p = lpNext(lp, p); /* Skip deleted field. */
        p = lpNext(lp, p); /* Skip num-of-fields in the master entry. */

        /* Skip all the master fields. */
        int64_t master_fields_count = lpGetInteger(p);
        p = lpNext(lp,p); /* Skip the first field. */
        for (int64_t j = 0; j < master_fields_count; j++)
            p = lpNext(lp,p); /* Skip all master fields. */
        p = lpNext(lp,p); /* Skip the zero master entry terminator. */

        /* 'p' is now pointing to the first entry inside the listpack.
         * We have to run entry after entry, marking entries as deleted
         * if they are already not deleted. */
        while (p) {
            /* We keep a copy of p (which point to flags part) in order to
             * update it after (and if) we actually remove the entry */
            unsigned char *pcopy = p;

            int64_t flags = lpGetInteger(p);
            p = lpNext(lp, p); /* Skip flags. */
            int64_t to_skip;

            int64_t ms_delta = lpGetInteger(p);
            p = lpNext(lp, p); /* Skip ID ms delta */
            int64_t seq_delta = lpGetInteger(p);
            p = lpNext(lp, p); /* Skip ID seq delta */

            streamID currid = {0}; /* For MINID */
            if (trim_strategy == TRIM_STRATEGY_MINID) {
                currid.ms = master_id.ms + ms_delta;
                currid.seq = master_id.seq + seq_delta;
            }

            int stop;
            if (trim_strategy == TRIM_STRATEGY_MAXLEN) {
                stop = s->length <= maxlen;
            } else {
                /* Following IDs will definitely be greater because the rax
                 * tree is sorted, no point of continuing. */
                stop = streamCompareID(&currid, id) >= 0;
            }
            if (stop)
                break;

            if (flags & STREAM_ITEM_FLAG_SAMEFIELDS) {
                to_skip = master_fields_count;
            } else {
                to_skip = lpGetInteger(p); /* Get num-fields. */
                p = lpNext(lp,p); /* Skip num-fields. */
                to_skip *= 2; /* Fields and values. */
            }

            while(to_skip--) p = lpNext(lp,p); /* Skip the whole entry. */
            p = lpNext(lp,p); /* Skip the final lp-count field. */

            /* Mark the entry as deleted. */
            if (!(flags & STREAM_ITEM_FLAG_DELETED)) {
                intptr_t delta = p - lp;
                flags |= STREAM_ITEM_FLAG_DELETED;
                lp = lpReplaceInteger(lp, &pcopy, flags);
                deleted_from_lp++;
                s->length--;
                p = lp + delta;
            }
        }
        deleted += deleted_from_lp;

        /* Now we update the entries/deleted counters. */
        p = lpFirst(lp);
        lp = lpReplaceInteger(lp,&p,entries-deleted_from_lp);
        p = lpNext(lp,p); /* Skip deleted field. */
        int64_t marked_deleted = lpGetInteger(p);
        lp = lpReplaceInteger(lp,&p,marked_deleted+deleted_from_lp);
        p = lpNext(lp,p); /* Skip num-of-fields in the master entry. */

        /* Here we should perform garbage collection in case at this point
         * there are too many entries deleted inside the listpack. */
        entries -= deleted_from_lp;
        marked_deleted += deleted_from_lp;
        if (entries + marked_deleted > 10 && marked_deleted > entries/2) {
            /* TODO: perform a garbage collection. */
        }

        /* Update the listpack with the new pointer. */
        raxInsert(s->rax,ri.key,ri.key_len,lp,NULL);

        break; /* If we are here, there was enough to delete in the current
                  node, so no need to go to the next node. */
    }
    raxStop(&ri);

    /* Update the stream's first ID after the trimming. */
    if (s->length == 0) {
        s->first_id.ms = 0;
        s->first_id.seq = 0;
    } else if (deleted) {
        streamGetEdgeID(s,1,1,&s->first_id);
    }

    return deleted;
}