any call(F fn, Args &&... args) {
  return fn(std::forward<Args>(args)...);
}