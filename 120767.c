bool Hub::listen(int port, uS::TLS::Context sslContext, int options, Group<SERVER> *eh) {
    if (!eh) {
        eh = (Group<SERVER> *) this;
    }

    if (uS::Node::listen<onServerAccept>(port, sslContext, options, (uS::NodeData *) eh, nullptr)) {
        eh->errorHandler(port);
        return false;
    }
    return true;
}