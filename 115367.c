inline std::shared_ptr<Ope> ign(const std::shared_ptr<Ope> &ope) {
  return std::make_shared<Ignore>(ope);
}