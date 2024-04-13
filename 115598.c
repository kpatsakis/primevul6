template <typename... Args> std::shared_ptr<Ope> cho(Args &&... args) {
  return std::make_shared<PrioritizedChoice>(
      static_cast<std::shared_ptr<Ope>>(args)...);
}