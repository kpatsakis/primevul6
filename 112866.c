String string_numeric_to_base(const Variant& value, int base) {
  static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

  assertx(string_validate_base(base));
  if ((!value.isInteger() && !value.isDouble())) {
    return empty_string();
  }

  if (value.isDouble()) {
    double fvalue = floor(value.toDouble()); /* floor it just in case */
    char *ptr, *end;
    char buf[(sizeof(double) << 3) + 1];

    /* Don't try to convert +/- infinity */
    if (fvalue == HUGE_VAL || fvalue == -HUGE_VAL) {
      raise_warning("Number too large");
      return empty_string();
    }

    end = ptr = buf + sizeof(buf) - 1;

    do {
      *--ptr = digits[(int) fmod(fvalue, base)];
      fvalue /= base;
    } while (ptr > buf && fabs(fvalue) >= 1);

    return String(ptr, end - ptr, CopyString);
  }

  return string_long_to_base(value.toInt64(), base);
}