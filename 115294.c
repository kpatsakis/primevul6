  CharacterClass(const std::string &s, bool negated) : negated_(negated) {
    auto chars = decode(s.c_str(), s.length());
    auto i = 0u;
    while (i < chars.size()) {
      if (i + 2 < chars.size() && chars[i + 1] == '-') {
        auto cp1 = chars[i];
        auto cp2 = chars[i + 2];
        ranges_.emplace_back(std::make_pair(cp1, cp2));
        i += 3;
      } else {
        auto cp = chars[i];
        ranges_.emplace_back(std::make_pair(cp, cp));
        i += 1;
      }
    }
    assert(!ranges_.empty());
  }