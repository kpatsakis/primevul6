template <typename... Args> std::shared_ptr<Ope> seq(Args &&... args) {
  return std::make_shared<Sequence>(static_cast<std::shared_ptr<Ope>>(args)...);
}