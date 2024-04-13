void FastCGIServer::timeoutExpired() noexcept {
  // Acceptor failed to drain connections on time; drop them so that we can
  // shutdown.
  if (m_acceptor) {
    m_acceptor->forceStop();
  }

  terminateServer();
}