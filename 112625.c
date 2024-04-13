static unsigned long php_turkish_tolower(unsigned long code, long l, long r,
                                         int field) {
  if (code == 0x0049L) {
    return 0x0131L;
  }
  return case_lookup(code, l, r, field);
}