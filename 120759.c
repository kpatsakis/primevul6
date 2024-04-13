void Hub::onServerAccept(uS::Socket s) {
    uS::SocketData *socketData = s.getSocketData();
    s.startTimeout<HTTPSocket<SERVER>::onEnd>();
    s.enterState<HTTPSocket<SERVER>>(new HTTPSocket<SERVER>::Data(socketData));
    delete socketData;
}