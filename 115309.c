ncls(const std::vector<std::pair<char32_t, char32_t>> &ranges) {
  return std::make_shared<CharacterClass>(ranges, true);
}