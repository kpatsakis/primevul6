String string_long_to_base(unsigned long value, int base) {
  static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  char buf[(sizeof(unsigned long) << 3) + 1];
  char *ptr, *end;

  assert(string_validate_base(base));

  end = ptr = buf + sizeof(buf) - 1;

  do {
    *--ptr = digits[value % base];
    value /= base;
  } while (ptr > buf && value);

  return String(ptr, end - ptr, CopyString);
}