String string_quoted_printable_encode(const char *input, int len) {
  size_t length = len;
  const unsigned char *str = (unsigned char*)input;

  unsigned long lp = 0;
  unsigned char c;
  char *d, *buffer;
  char *hex = "0123456789ABCDEF";

  String ret(
    safe_address(
      3,
      length + ((safe_address(3, length, 0)/(PHP_QPRINT_MAXL-9)) + 1),
      1),
    ReserveString
  );
  d = buffer = ret.mutableData();

  while (length--) {
    if (((c = *str++) == '\015') && (*str == '\012') && length > 0) {
      *d++ = '\015';
      *d++ = *str++;
      length--;
      lp = 0;
    } else {
      if (iscntrl (c) || (c == 0x7f) || (c & 0x80) ||
          (c == '=') || ((c == ' ') && (*str == '\015'))) {
        if ((((lp+= 3) > PHP_QPRINT_MAXL) && (c <= 0x7f))
            || ((c > 0x7f) && (c <= 0xdf) && ((lp + 3) > PHP_QPRINT_MAXL))
            || ((c > 0xdf) && (c <= 0xef) && ((lp + 6) > PHP_QPRINT_MAXL))
            || ((c > 0xef) && (c <= 0xf4) && ((lp + 9) > PHP_QPRINT_MAXL))) {
          *d++ = '=';
          *d++ = '\015';
          *d++ = '\012';
          lp = 3;
        }
        *d++ = '=';
        *d++ = hex[c >> 4];
        *d++ = hex[c & 0xf];
      } else {
        if ((++lp) > PHP_QPRINT_MAXL) {
          *d++ = '=';
          *d++ = '\015';
          *d++ = '\012';
          lp = 1;
        }
        *d++ = c;
      }
    }
  }
  len = d - buffer;

  ret.setSize(len);
  return ret;
}