void streamNextID(streamID *last_id, streamID *new_id) {
    uint64_t ms = mstime();
    if (ms > last_id->ms) {
        new_id->ms = ms;
        new_id->seq = 0;
    } else {
        *new_id = *last_id;
        streamIncrID(new_id);
    }
}