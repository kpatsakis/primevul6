inline std::shared_ptr<Ope> cls(const std::string &s) {
  return std::make_shared<CharacterClass>(s, false);
}