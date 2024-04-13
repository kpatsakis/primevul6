String string_money_format(const char *format, double value) {
  bool check = false;
  const char *p = format;
  while ((p = strchr(p, '%'))) {
    if (*(p + 1) == '%') {
      p += 2;
    } else if (!check) {
      check = true;
      p++;
    } else {
      throw_invalid_argument
        ("format: Only a single %%i or %%n token can be used");
      return String();
    }
  }

  int format_len = strlen(format);
  int str_len = safe_address(format_len, 1, 1024);
  String ret(str_len, ReserveString);
  char *str = ret.mutableData();
  if ((str_len = strfmon(str, str_len, format, value)) < 0) {
    return String();
  }
  ret.setSize(str_len);
  return ret;
}