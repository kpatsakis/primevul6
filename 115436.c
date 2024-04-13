usr(std::function<size_t(const char *s, size_t n, SemanticValues &sv, any &dt)>
        fn) {
  return std::make_shared<User>(fn);
}