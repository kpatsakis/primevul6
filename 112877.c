void FastCGIServer::waitForEnd() {
  // When m_worker stops the server has stopped accepting new requests, there
  // may be pedning vm jobs. wait() is always safe to call regardless of thread
  m_worker.wait();
}