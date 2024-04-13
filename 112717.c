String StringUtil::MoneyFormat(const char *format, double value) {
  assert(format);
  return string_money_format(format, value);
}