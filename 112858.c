String string_base64_encode(const char* input, int len) {
  if (auto const wantedSize = maxEncodedSize(len)) {
    String ret(*wantedSize, ReserveString);
    auto actualSize = php_base64_encode((unsigned char*)input, len,
                                        (unsigned char*)ret.mutableData());
    ret.setSize(actualSize);
    return ret;
  }
  return String();
}