size_t safe_address(size_t nmemb, size_t size, size_t offset) {
  uint64_t result =
    (uint64_t) nmemb * (uint64_t) size + (uint64_t) offset;
  if (UNLIKELY(result > StringData::MaxSize)) {
    throw
      FatalErrorException(0, "String length exceeded 2^31-2: %" PRIu64, result);
  }
  return result;
}