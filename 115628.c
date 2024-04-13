  CharacterClass(const std::vector<std::pair<char32_t, char32_t>> &ranges,
                 bool negated)
      : ranges_(ranges), negated_(negated) {
    assert(!ranges_.empty());
  }