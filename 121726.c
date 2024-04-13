[[noreturn]] void TProtocolException::throwUnionMissingStop() {
  throw TProtocolException(
      TProtocolException::INVALID_DATA,
      "missing stop marker to terminate a union");
}