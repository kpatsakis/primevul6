  static std::shared_ptr<Grammar> parse(const char *s, size_t n,
                                        const Rules &rules, std::string &start,
                                        Log log) {
    return get_instance().perform_core(s, n, rules, start, log);
  }