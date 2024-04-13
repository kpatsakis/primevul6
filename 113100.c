size_t HTTP2Codec::generateConnectionPreface(folly::IOBufQueue& writeBuf) {
  if (transportDirection_ == TransportDirection::UPSTREAM) {
    VLOG(4) << "generating connection preface";
    writeBuf.append(http2::kConnectionPreface);
    return http2::kConnectionPreface.length();
  }
  return 0;
}