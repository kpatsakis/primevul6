void FastCGITransport::onSendEndImpl() {
  // Don't send onComplete() more than once (required because of the eom flag
  // on sendImpl).
  if (m_sendEnded) {
    return;
  }

  m_sendEnded = true;
  // NB: onSendEnd() is only sent when the VM is finished with the transport.
  //     at this point we are free to do whatever we'd like with the transport.
  m_session->onComplete();
}