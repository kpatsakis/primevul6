int streamIDEqZero(streamID *id) {
    return !(id->ms || id->seq);
}