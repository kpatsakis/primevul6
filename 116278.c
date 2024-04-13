void print_error(const char *file, int line, const char *format,
                 const Args & ... args) {
  fmt::print("{}: {}: ", file, line);
  fmt::print(format, args...);
}