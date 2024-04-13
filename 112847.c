String string_base64_decode(const char* input, int len, bool strict) {
  String ret(len, ReserveString);
  auto actualSize = php_base64_decode(input, len, strict,
                                      (unsigned char*)ret.mutableData());
  if (actualSize < 0) return String();

  ret.setSize(actualSize);
  return ret;
}