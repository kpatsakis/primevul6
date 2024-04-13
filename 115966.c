  static std::string format(fmt::CStringRef format_str, const Args & ... args) {
    return fmt::format(format_str, args...);
  }