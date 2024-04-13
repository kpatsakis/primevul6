  static Value make(const T &value) {
#ifdef __clang__
    Value result = MakeValue<Formatter>(value);
    // Workaround a bug in Apple LLVM version 4.2 (clang-425.0.28) of clang:
    // https://github.com/fmtlib/fmt/issues/276
    (void)result.custom.format;
    return result;
#else
    return MakeValue<Formatter>(value);
#endif
  }