inline std::shared_ptr<Ope> liti(std::string &&s) {
  return std::make_shared<LiteralString>(s, true);
}