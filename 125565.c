void addReplyStreamID(client *c, streamID *id) {
    addReplyBulkSds(c,createStreamIDString(id));
}