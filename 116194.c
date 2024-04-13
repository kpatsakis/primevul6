inline uint64_t make_type(const Arg &first, const Args & ... tail) {
  return make_type(first) | (make_type(tail...) << 4);
}