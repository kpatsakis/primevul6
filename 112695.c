void FastCGITransport::getHeaders(HeaderMap& headers) {
  for (auto& pair : m_requestParams) {
    auto key = unmangleHeader(pair.first);
    if (!key.empty()) {
      headers[key] = { pair.second };
    }
  }
}