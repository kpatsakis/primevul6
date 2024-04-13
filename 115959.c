inline uint64_t make_type(const T &arg) {
  return MakeValue< BasicFormatter<char> >::type(arg);
}