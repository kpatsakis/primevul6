void FastCGITransport::addHeaderImpl(const char* name, const char* value) {
  CHECK(!m_headersSent);

  m_responseHeaders[name].emplace_back(value);
}