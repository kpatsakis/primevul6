bool WebSocketProtocol<isServer>::refusePayloadLength(void *user, int length) {
    return length > 16777216;
}