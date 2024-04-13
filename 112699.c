void FastCGITransport::sendImpl(const void *data, int size, int code,
                                bool chunked, bool eom) {
  if (!m_headersSent) {
    m_headersSent = true;
    sendResponseHeaders(m_txBuf, code);
  }

  m_txBuf.append(data, size);
  m_session->onStdOut(m_txBuf.move()); // session will handle locking

  if (eom) {
    onSendEndImpl();
  }
}