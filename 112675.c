static StringData* asciiToLower(const StringData* s) {
  const auto size = s->size();
  auto ret = StringData::Make(s, CopyString);
  auto output = ret->mutableData();
  for (int i = 0; i < size; ++i) {
    auto& c = output[i];
    if (c <= 'Z' && c >= 'A') {
      c |= 0x20;
    }
  }
  ret->invalidateHash(); // We probably modified it.
  return ret;
}