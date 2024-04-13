static char to_hex(char code) {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
}