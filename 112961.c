void HTTP2Codec::onHeader(const folly::fbstring& name,
                          const folly::fbstring& value) {
  if (decodeInfo_.onHeader(name, value)) {
    if (name == "user-agent" && userAgent_.empty()) {
      userAgent_ = value.toStdString();
    }
  } else {
    VLOG(4) << "dir=" << uint32_t(transportDirection_) <<
      decodeInfo_.parsingError << " codec=" << headerCodec_;
  }
}