void FastCGIServer::start() {
  // It's not safe to call this function more than once
  m_socket.reset(new folly::AsyncServerSocket(m_worker.getEventBase()));
  try {
    m_socket->bind(m_socketConfig.bindAddress);
  } catch (const std::system_error& ex) {
    Logger::Error(std::string(ex.what()));
    if (m_socketConfig.bindAddress.getFamily() == AF_UNIX) {
      throw FailedToListenException(m_socketConfig.bindAddress.getPath());
    }
    throw FailedToListenException(m_socketConfig.bindAddress.getAddressStr(),
                                  m_socketConfig.bindAddress.getPort());
  }
  if (m_socketConfig.bindAddress.getFamily() == AF_UNIX) {
    auto path = m_socketConfig.bindAddress.getPath();
    chmod(path.c_str(), 0760);
  }
  m_acceptor.reset(new FastCGIAcceptor(m_socketConfig, this));
  m_acceptor->init(m_socket.get(), m_worker.getEventBase());
  m_worker.getEventBase()->runInEventBaseThread([&] {
    if (!m_socket) {
      // Someone called stop before we got here. With the exception of a
      // second call to start being made this should be safe as any place
      // we mutate m_socket is done within the event base.
      return;
    }
    m_socket->listen(m_socketConfig.acceptBacklog);
    m_socket->startAccepting();
  });
  setStatus(RunStatus::RUNNING);
  folly::AsyncTimeout::attachEventBase(m_worker.getEventBase());
  m_worker.start();
  m_dispatcher.start();
}