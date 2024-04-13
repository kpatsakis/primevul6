void string_to_case(String& s, int (*tocase)(int)) {
  assertx(!s.isNull());
  assertx(tocase);
  auto data = s.mutableData();
  auto len = s.size();
  for (int i = 0; i < len; i++) {
    data[i] = tocase(data[i]);
  }
}