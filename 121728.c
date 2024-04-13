[[noreturn]] void TProtocolException::throwExceededSizeLimit() {
  throw TProtocolException(TProtocolException::SIZE_LIMIT);
}