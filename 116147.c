ArgJoin<wchar_t, It> join(It first, It last, const BasicCStringRef<wchar_t>& sep) {
  return ArgJoin<wchar_t, It>(first, last, sep);
}