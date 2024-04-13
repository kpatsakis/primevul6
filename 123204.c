TLSContent PlaintextWriteRecordLayer::writeInitialClientHello(
    Buf encodedClientHello) const {
  return write(
      TLSMessage{ContentType::handshake, std::move(encodedClientHello)},
      ProtocolVersion::tls_1_0);
}