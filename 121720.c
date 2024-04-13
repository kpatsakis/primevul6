[[noreturn]] void TProtocolException::throwMissingRequiredField(
    folly::StringPiece field,
    folly::StringPiece type) {
  constexpr auto fmt =
      "Required field '{}' was not found in serialized data! Struct: {}";
  throw TProtocolException(
      TProtocolException::MISSING_REQUIRED_FIELD,
      folly::sformat(fmt, field, type));
}