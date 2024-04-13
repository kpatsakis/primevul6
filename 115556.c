inline std::shared_ptr<Ope> apd(const std::shared_ptr<Ope> &ope) {
  return std::make_shared<AndPredicate>(ope);
}