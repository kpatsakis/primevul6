void setDeferredReplyStreamID(client *c, void *dr, streamID *id) {
    setDeferredReplyBulkSds(c, dr, createStreamIDString(id));
}