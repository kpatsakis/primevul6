inline bool is_negative(T value) {
  return SignChecker<std::numeric_limits<T>::is_signed>::is_negative(value);
}