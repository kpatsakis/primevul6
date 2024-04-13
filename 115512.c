  DetectInfiniteLoop(const char *s, const std::string &name) {
    refs_.emplace_back(s, name);
  }