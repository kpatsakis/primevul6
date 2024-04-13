void FastCGITransport::getTransportParams(HeaderMap& serverParams) {
  for (auto& pair : m_requestParams) {
    serverParams[pair.first] = { pair.second };
  }
}