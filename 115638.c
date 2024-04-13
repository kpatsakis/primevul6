any call(F fn, Args &&... args) {
  return any(fn(std::forward<Args>(args)...));
}