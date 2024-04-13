bool Hub::upgrade(uv_os_sock_t fd, const char *secKey, SSL *ssl, const char *extensions, size_t extensionsLength, Group<SERVER> *serverGroup) {
    if (!serverGroup) {
        serverGroup = &getDefaultGroup<SERVER>();
    }

    uS::Socket s = uS::Socket::init((uS::NodeData *) serverGroup, fd, ssl);
    uS::SocketData *socketData = s.getSocketData();
    HTTPSocket<SERVER>::Data *temporaryHttpData = new HTTPSocket<SERVER>::Data(socketData);
    delete socketData;
    s.enterState<HTTPSocket<SERVER>>(temporaryHttpData);

    // todo: move this into upgrade call
    bool perMessageDeflate = false;
    std::string extensionsResponse;
    if (extensionsLength) {
        ExtensionsNegotiator<uWS::SERVER> extensionsNegotiator(serverGroup->extensionOptions);
        extensionsNegotiator.readOffer(std::string(extensions, extensionsLength));
        extensionsResponse = extensionsNegotiator.generateOffer();
        if (extensionsNegotiator.getNegotiatedOptions() & PERMESSAGE_DEFLATE) {
            perMessageDeflate = true;
        }
    }

    if (HTTPSocket<SERVER>(s).upgrade(secKey, extensionsResponse.data(), extensionsResponse.length())) {
        s.enterState<WebSocket<SERVER>>(new WebSocket<SERVER>::Data(perMessageDeflate, s.getSocketData()));
        serverGroup->addWebSocket(s);
        serverGroup->connectionHandler(WebSocket<SERVER>(s), {nullptr, nullptr, 0, 0});
        delete temporaryHttpData;
        return true;
    }
    return false;
}