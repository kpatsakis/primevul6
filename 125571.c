long long streamEstimateDistanceFromFirstEverEntry(stream *s, streamID *id) {
    /* The counter of any ID in an empty, never-before-used stream is 0. */
    if (!s->entries_added) {
        return 0;
    }

    /* In the empty stream, if the ID is smaller or equal to the last ID,
     * it can set to the current added_entries value. */
    if (!s->length && streamCompareID(id,&s->last_id) < 1) {
        return s->entries_added;
    }

    int cmp_last = streamCompareID(id,&s->last_id);
    if (cmp_last == 0) {
        /* Return the exact counter of the last entry in the stream. */
        return s->entries_added;
    } else if (cmp_last > 0) {
        /* The counter of a future ID is unknown. */
        return SCG_INVALID_ENTRIES_READ;
    }

    int cmp_id_first = streamCompareID(id,&s->first_id);
    int cmp_xdel_first = streamCompareID(&s->max_deleted_entry_id,&s->first_id);
    if (streamIDEqZero(&s->max_deleted_entry_id) || cmp_xdel_first < 0) {
        /* There's definitely no fragmentation ahead. */
        if (cmp_id_first < 0) {
            /* Return the estimated counter. */
            return s->entries_added - s->length;
        } else if (cmp_id_first == 0) {
            /* Return the exact counter of the first entry in the stream. */
            return s->entries_added - s->length + 1;
        }
    }

    /* The ID is either before an XDEL that fragments the stream or an arbitrary
     * ID. Either case, so we can't make a prediction. */
    return SCG_INVALID_ENTRIES_READ;
}