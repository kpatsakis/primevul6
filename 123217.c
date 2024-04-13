TLSContent PlaintextWriteRecordLayer::write(TLSMessage&& msg) const {
  return write(std::move(msg), ProtocolVersion::tls_1_2);
}