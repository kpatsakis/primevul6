void FastCGIServer::stop() {
  if (getStatus() != RunStatus::RUNNING) return; // nothing to do

  setStatus(RunStatus::STOPPING);
  HttpServer::MarkShutdownStat(ShutdownEvent::SHUTDOWN_DRAIN_READS);

  m_worker.getEventBase()->runInEventBaseThread([&] {
    // Shutdown the server socket. Unfortunately, we will drop all unaccepted
    // connections; there is no way to do a partial shutdown of a server socket
    m_socket->stopAccepting();

    if (RuntimeOption::ServerGracefulShutdownWait > 0) {
      // Gracefully drain any incomplete requests. We cannot go offline until
      // they are finished as we own their dispatcher and event base.
      if (m_acceptor) {
        m_acceptor->drainAllConnections();
      }

      std::chrono::seconds s(RuntimeOption::ServerGracefulShutdownWait);
      std::chrono::milliseconds m(s);
      scheduleTimeout(m);
    } else {
      // Drop all connections. We cannot shutdown until they stop because we
      // own their dispatcher and event base.
      if (m_acceptor) {
        m_acceptor->forceStop();
      }

      terminateServer();
    }
  });
}