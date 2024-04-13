void FastCGIServer::terminateServer() {
  if (getStatus() != RunStatus::STOPPING) {
    setStatus(RunStatus::STOPPING);
  }
  // Wait for the server socket thread to stop running
  m_worker.stopWhenIdle();

  HttpServer::MarkShutdownStat(ShutdownEvent::SHUTDOWN_DRAIN_DISPATCHER);
  // Wait for VMs to shutdown
  m_dispatcher.stop();

  setStatus(RunStatus::STOPPED);
  HttpServer::MarkShutdownStat(ShutdownEvent::SHUTDOWN_DONE);

  // Notify HttpServer that we've shutdown
  for (auto listener: m_listeners) {
    listener->serverStopped(this);
  }
}