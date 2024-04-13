inline void format_decimal(Char *buffer, UInt value, unsigned num_digits) {
  format_decimal(buffer, value, num_digits, NoThousandsSep());
  return;
}