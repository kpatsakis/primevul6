std::unique_ptr<folly::IOBuf> HTTP2Codec::encodeHeaders(
    const HTTPHeaders& headers,
    std::vector<compress::Header>& allHeaders,
    HTTPHeaderSize* size) {
  headerCodec_.setEncodeHeadroom(http2::kFrameHeaderSize +
                                 http2::kFrameHeadersBaseMaxSize);
  auto out = headerCodec_.encode(allHeaders);
  if (size) {
    *size = headerCodec_.getEncodedSize();
  }

  if (headerCodec_.getEncodedSize().uncompressed >
      ingressSettings_.getSetting(SettingsId::MAX_HEADER_LIST_SIZE,
                                  std::numeric_limits<uint32_t>::max())) {
    // The remote side told us they don't want headers this large...
    // but this function has no mechanism to fail
    string serializedHeaders;
    headers.forEach(
      [&serializedHeaders] (const string& name, const string& value) {
        serializedHeaders = folly::to<string>(serializedHeaders, "\\n", name,
                                              ":", value);
      });
    LOG(ERROR) << "generating HEADERS frame larger than peer maximum nHeaders="
               << headers.size() << " all headers="
               << serializedHeaders;
  }
  return out;
}