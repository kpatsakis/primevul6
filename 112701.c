bool FastCGITransport::hasMorePostData() {
  Lock lock(this);
  return !m_bodyComplete || !m_bodyQueue.empty();
}