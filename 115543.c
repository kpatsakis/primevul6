  static std::shared_ptr<Grammar> parse(const char *s, size_t n,
                                        std::string &start, Log log) {
    Rules dummy;
    return parse(s, n, dummy, start, log);
  }