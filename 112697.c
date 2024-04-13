const void *FastCGITransport::getPostData(size_t& size) {
  // the API contract is that you can call getPostData repeatedly until
  // you call getMorePostData
  if (m_firstBody) {
    CHECK(m_currBody);
    size = m_currBody->length();
    return m_currBody->data();
  }
  return getMorePostData(size);
}