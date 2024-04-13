void WebSocketProtocol<isServer>::forceClose(void *user) {
    WebSocket<isServer>((uv_poll_t *) user).terminate();
}