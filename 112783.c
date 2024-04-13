String string_base64_encode(const char *input, int len) {
  return php_base64_encode((unsigned char *)input, len);
}