bool FastCGIAcceptor::canAccept(const folly::SocketAddress& /*address*/) {
  // TODO: Support server IP whitelist.
  auto const cons = m_server->getLibEventConnectionCount();
  return (RuntimeOption::ServerConnectionLimit == 0 ||
          cons < RuntimeOption::ServerConnectionLimit);
}