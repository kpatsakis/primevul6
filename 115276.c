inline std::shared_ptr<Ope> npd(const std::shared_ptr<Ope> &ope) {
  return std::make_shared<NotPredicate>(ope);
}