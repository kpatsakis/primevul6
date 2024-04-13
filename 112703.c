void FastCGITransport::onBodyComplete() {
  Lock lock(this);
  m_bodyComplete = true;
  notify(); // wake-up the VM
}