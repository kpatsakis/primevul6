void string_to_case(String& s, int (*tocase)(int)) {
  assert(!s.isNull());
  assert(tocase);
  auto data = s.bufferSlice().ptr;
  auto len = s.size();
  for (int i = 0; i < len; i++) {
    data[i] = tocase(data[i]);
  }
}