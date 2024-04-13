void Hub::onClientConnection(uS::Socket s, bool error) {
    HTTPSocket<CLIENT>::Data *httpSocketData = (HTTPSocket<CLIENT>::Data *) s.getSocketData();

    if (error) {
        ((Group<CLIENT> *) httpSocketData->nodeData)->errorHandler(httpSocketData->httpUser);
        delete httpSocketData;
    } else {
        s.enterState<HTTPSocket<CLIENT>>(s.getSocketData());
        HTTPSocket<CLIENT>(s).upgrade();
    }
}