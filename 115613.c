inline std::shared_ptr<Ope> bkr(const std::string &name) {
  return std::make_shared<BackReference>(name);
}