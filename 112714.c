int64_t StringUtil::CRC32(const String& input) {
  return string_crc32(input.data(), input.size());
}