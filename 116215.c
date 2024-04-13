inline internal::NamedArgWithType<wchar_t, T> arg(WStringRef name, const T &arg) {
  return internal::NamedArgWithType<wchar_t, T>(name, arg);
}