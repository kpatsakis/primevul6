[[noreturn]] void TProtocolException::throwInvalidSkipType(TType type) {
  throw TProtocolException(
      TProtocolException::INVALID_DATA,
      folly::sformat(
          "Encountered invalid field/element type ({}) during skipping",
          static_cast<uint8_t>(type)));
}