void Hub::connect(std::string uri, void *user, int timeoutMs, Group<CLIENT> *eh) {
    if (!eh) {
        eh = (Group<CLIENT> *) this;
    }

    int offset = 0;
    std::string protocol = uri.substr(offset, uri.find("://")), hostname, portStr, path;
    if ((offset += protocol.length() + 3) < uri.length()) {
        hostname = uri.substr(offset, uri.find_first_of(":/", offset) - offset);

        offset += hostname.length();
        if (uri[offset] == ':') {
            offset++;
            portStr = uri.substr(offset, uri.find("/", offset) - offset);
        }

        offset += portStr.length();
        if (uri[offset] == '/') {
            path = uri.substr(++offset);
        }
    }

    if (hostname.length()) {
        int port = 80;
        bool secure = false;
        if (protocol == "wss") {
            port = 443;
            secure = true;
        } else if (protocol != "ws") {
            eh->errorHandler(user);
        }

        if (portStr.length()) {
            port = stoi(portStr);
        }

        uS::SocketData socketData((uS::NodeData *) eh);
        HTTPSocket<CLIENT>::Data *httpSocketData = new HTTPSocket<CLIENT>::Data(&socketData);

        httpSocketData->host = hostname;
        httpSocketData->path = path;
        httpSocketData->httpUser = user;

        uS::Socket s = uS::Node::connect<onClientConnection>(hostname.c_str(), port, secure, httpSocketData);
        if (s) {
            s.startTimeout<HTTPSocket<CLIENT>::onEnd>(timeoutMs);
        }
    } else {
        eh->errorHandler(user);
    }
}