std::string base64_encode(const char* input, int len) {
  if (auto const wantedSize = maxEncodedSize(len)) {
    std::string ret;
    ret.resize(*wantedSize);
    auto actualSize = php_base64_encode((unsigned char*)input, len,
                                        (unsigned char*)ret.data());
    ret.resize(actualSize);
    return ret;
  }
  return std::string();
}