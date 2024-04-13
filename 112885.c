void FastCGIServer::onConnectionsDrained() {
  // NOTE: called from FastCGIAcceptor::onConnectionsDrained()
  cancelTimeout();
  terminateServer();
}