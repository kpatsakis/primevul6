  static std::string format(fmt::CStringRef format_str, const Args & ... args) {
    return TestFormat<N - 1>::format(format_str, N - 1, args...);
  }