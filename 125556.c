int streamRangeHasTombstones(stream *s, streamID *start, streamID *end) {
    streamID start_id, end_id;

    if (!s->length || streamIDEqZero(&s->max_deleted_entry_id)) {
        /* The stream is empty or has no tombstones. */
        return 0;
    }

    if (streamCompareID(&s->first_id,&s->max_deleted_entry_id) > 0) {
        /* The latest tombstone is before the first entry. */
        return 0;
    }

    if (start) {
        start_id = *start;
    } else {
        start_id.ms = 0;
        start_id.seq = 0;
    }

    if (end) {
        end_id = *end;
    } else {
        end_id.ms = UINT64_MAX;
        end_id.seq = UINT64_MAX;
    }

    if (streamCompareID(&start_id,&s->max_deleted_entry_id) <= 0 &&
        streamCompareID(&s->max_deleted_entry_id,&end_id) <= 0)
    {
        /* start_id <= max_deleted_entry_id <= end_id: The range does include a tombstone. */
        return 1;
    }

    /* The range doesn't includes a tombstone. */
    return 0;
}