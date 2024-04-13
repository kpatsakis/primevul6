[[noreturn]] void TProtocolException::throwBoolValueOutOfRange(uint8_t value) {
  throw TProtocolException(
      TProtocolException::INVALID_DATA,
      folly::sformat(
          "Attempt to interpret value {} as bool, probably the data is corrupted",
          value));
}