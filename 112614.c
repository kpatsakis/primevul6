static unsigned long php_turkish_toupper(unsigned long code, long l, long r,
                                         int field) {
  if (code == 0x0069L) {
    return 0x0130L;
  }
  return case_lookup(code, l, r, field);
}