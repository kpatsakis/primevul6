std::string format_decimal(T value) {
  char buffer[10];
  char *ptr = buffer;
  fmt::format_decimal(ptr, value);
  return std::string(buffer, ptr);
}