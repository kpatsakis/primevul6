int64_t streamTrimByLength(stream *s, long long maxlen, int approx) {
    streamAddTrimArgs args = {
        .trim_strategy = TRIM_STRATEGY_MAXLEN,
        .approx_trim = approx,
        .limit = approx ? 100 * server.stream_node_max_entries : 0,
        .maxlen = maxlen
    };
    return streamTrim(s, &args);
}