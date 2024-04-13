void check_unknown_types(
    const T &value, const char *types, const char *type_name) {
  char format_str[BUFFER_SIZE], message[BUFFER_SIZE];
  const char *special = ".0123456789}";
  for (int i = CHAR_MIN; i <= CHAR_MAX; ++i) {
    char c = static_cast<char>(i);
    if (std::strchr(types, c) || std::strchr(special, c) || !c) continue;
    safe_sprintf(format_str, "{0:10%c}", c);
    if (std::isprint(static_cast<unsigned char>(c))) {
      safe_sprintf(message, "unknown format code '%c' for %s", c, type_name);
    } else {
      safe_sprintf(message, "unknown format code '\\x%02x' for %s", c,
                   type_name);
    }
    EXPECT_THROW_MSG(format(format_str, value), FormatError, message)
      << format_str << " " << message;
  }
}