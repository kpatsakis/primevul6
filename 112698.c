void FastCGITransport::removeHeaderImpl(const char* name) {
  CHECK(!m_headersSent);

  m_responseHeaders.erase(name);
}