void FastCGITransport::onBody(std::unique_ptr<folly::IOBuf> chain) {
  Lock lock(this);
  m_bodyQueue.append(std::move(chain));
  notify(); // wake-up the VM
}