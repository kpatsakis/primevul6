void FastCGIAcceptor::onNewConnection(
  folly::AsyncTransportWrapper::UniquePtr sock,
  const folly::SocketAddress* peerAddress,
  const std::string& /*nextProtocolName*/,
  SecureTransportType /*secureProtocolType*/,
  const ::wangle::TransportInfo& /*tinfo*/) {
  folly::SocketAddress localAddress;
  try {
    sock->getLocalAddress(&localAddress);
  } catch (std::system_error& e) {
    // If getSockName fails it's bad news; abort the connection
    return;
  }

  // Will delete itself when it gets a closing callback
  auto session = new FastCGISession(
      m_server->getEventBaseManager()->getExistingEventBase(),
      m_server->getDispatcher(),
      std::move(sock),
      localAddress,
      *peerAddress
    );

  // NB: ~ManagedConnection will call removeConnection() before the session
  //     destroys itself.
  Acceptor::addConnection(session);
};