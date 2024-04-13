inline std::shared_ptr<Ope> ncls(const std::string &s) {
  return std::make_shared<CharacterClass>(s, true);
}