bool WebSocketProtocol<isServer>::setCompressed(void *user) {
    uS::Socket s((uv_poll_t *) user);
    typename WebSocket<isServer>::Data *webSocketData = (typename WebSocket<isServer>::Data *) s.getSocketData();

    if (webSocketData->compressionStatus == WebSocket<isServer>::Data::CompressionStatus::ENABLED) {
        webSocketData->compressionStatus = WebSocket<isServer>::Data::CompressionStatus::COMPRESSED_FRAME;
        return true;
    } else {
        return false;
    }
}