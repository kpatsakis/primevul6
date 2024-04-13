  NamedArgWithType<Char, T> operator=(T &&value) const {
    return {str, std::forward<T>(value)};
  }