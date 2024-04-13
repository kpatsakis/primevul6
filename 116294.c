inline void require_numeric_argument(const Arg &arg, char spec) {
  if (arg.type > Arg::LAST_NUMERIC_TYPE) {
    std::string message =
        fmt::format("format specifier '{}' requires numeric argument", spec);
    FMT_THROW(fmt::FormatError(message));
  }
}