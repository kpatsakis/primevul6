void HTTP2Codec::onDecodeError(HPACK::DecodeError decodeError) {
  decodeInfo_.decodeError = decodeError;
}