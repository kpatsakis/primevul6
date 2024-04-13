[[noreturn]] void TProtocolException::throwNegativeSize() {
  throw TProtocolException(TProtocolException::NEGATIVE_SIZE);
}