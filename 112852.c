Variant string_base_to_numeric(const char *s, int len, int base) {
  int64_t num = 0;
  double fnum = 0;
  int mode = 0;
  int64_t cutoff;
  int cutlim;

  assertx(string_validate_base(base));

  cutoff = LONG_MAX / base;
  cutlim = LONG_MAX % base;

  for (int i = len; i > 0; i--) {
    char c = *s++;

    /* might not work for EBCDIC */
    if (c >= '0' && c <= '9')
      c -= '0';
    else if (c >= 'A' && c <= 'Z')
      c -= 'A' - 10;
    else if (c >= 'a' && c <= 'z')
      c -= 'a' - 10;
    else
      continue;

    if (c >= base)
      continue;

    switch (mode) {
    case 0: /* Integer */
      if (num < cutoff || (num == cutoff && c <= cutlim)) {
        num = num * base + c;
        break;
      } else {
        fnum = num;
        mode = 1;
      }
      /* fall-through */
    case 1: /* Float */
      fnum = fnum * base + c;
    }
  }

  if (mode == 1) {
    return fnum;
  }
  return num;
}