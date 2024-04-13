const void *FastCGITransport::getMorePostData(size_t& size) {
  // session will terminate the request if we don't receive data in
  // this much time
  long maxWait = RuntimeOption::ConnectionTimeoutSeconds;
  if (maxWait <= 0) {
    maxWait = 50; // this was the default read timeout in LibEventServer
  }

  Lock lock(this);
  while (m_bodyQueue.empty() && !m_bodyComplete) {
    wait(maxWait);
  }

  // For chunk encodings, we way receive an EOM with no data, such that
  // hasMorePostData returns true (because client is not yet complete),
  // client sends EOM, getMorePostData should return 0/nullptr
  size = 0;
  if (!m_bodyQueue.empty()) {
    // this is the first body if it wasn't set and buf is unset
    m_firstBody = !(m_firstBody && m_currBody);
    m_currBody = m_bodyQueue.pop_front();

    CHECK(m_currBody && m_currBody->length() > 0);
    size = m_currBody->length();
    return m_currBody->data();
  }

  return nullptr;
}