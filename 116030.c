void custom_format(const char *format_str, fmt::ArgList args) {
  fmt::MemoryWriter writer;
  fmt::BasicFormatter<char, MockArgFormatter> formatter(args, writer);
  formatter.format(format_str);
}