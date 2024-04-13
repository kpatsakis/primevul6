inline internal::NamedArgWithType<char, T> arg(StringRef name, const T &arg) {
  return internal::NamedArgWithType<char, T>(name, arg);
}