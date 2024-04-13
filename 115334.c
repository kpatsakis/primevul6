auto any_cast(Args &&... args)
    -> decltype(std::any_cast<T>(std::forward<Args>(args)...)) {
  return std::any_cast<T>(std::forward<Args>(args)...);
}