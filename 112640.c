static StringData* asciiToUpper(const StringData* s) {
  const auto size = s->size();
  auto ret = StringData::Make(s, CopyString);
  auto output = ret->mutableData();
  for (int i = 0; i < size; ++i) {
    auto& c = output[i];
    if (c >= 'a' && c <= 'z') {
      c -= (char)0x20;
    }
  }
  ret->invalidateHash(); // We probably modified it.
  return ret;
}