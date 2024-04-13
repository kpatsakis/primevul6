std::string base64_decode(const char* input, int len, bool strict) {
  if (!len) return std::string();
  std::string ret;
  ret.resize(len);
  auto actualSize = php_base64_decode(input, len, strict,
                                      (unsigned char*)ret.data());
  if (!actualSize) return std::string();

  ret.resize(actualSize);
  return ret;
}