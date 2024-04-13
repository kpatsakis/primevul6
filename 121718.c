[[noreturn]] void TProtocolException::throwReportedTypeMismatch() {
  throw TProtocolException(
      TProtocolException::INVALID_DATA,
      "The reported type of thrift element does not match the serialized type");
}