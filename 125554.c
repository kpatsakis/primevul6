int64_t streamTrimByID(stream *s, streamID minid, int approx) {
    streamAddTrimArgs args = {
        .trim_strategy = TRIM_STRATEGY_MINID,
        .approx_trim = approx,
        .limit = approx ? 100 * server.stream_node_max_entries : 0,
        .minid = minid
    };
    return streamTrim(s, &args);
}