TLSContent PlaintextWriteRecordLayer::write(
    TLSMessage msg,
    ProtocolVersion recordVersion) const {
  if (msg.type == ContentType::application_data) {
    throw std::runtime_error("refusing to send plaintext application data");
  }

  auto fragment = std::move(msg.fragment);
  folly::io::Cursor cursor(fragment.get());
  std::unique_ptr<folly::IOBuf> data;
  while (!cursor.isAtEnd()) {
    Buf thisFragment;
    auto len = cursor.cloneAtMost(thisFragment, kMaxPlaintextRecordSize);

    auto header = folly::IOBuf::create(kPlaintextHeaderSize);
    folly::io::Appender appender(header.get(), kPlaintextHeaderSize);
    appender.writeBE(static_cast<ContentTypeType>(msg.type));
    appender.writeBE(static_cast<ProtocolVersionType>(recordVersion));
    appender.writeBE<uint16_t>(len);

    if (!data) {
      data = std::move(header);
    } else {
      data->prependChain(std::move(header));
    }
    data->prependChain(std::move(thisFragment));
  }
  TLSContent content;
  content.data = std::move(data);
  content.contentType = msg.type;
  content.encryptionLevel = EncryptionLevel::Plaintext;
  return content;
}