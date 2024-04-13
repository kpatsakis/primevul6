ArgJoin<char, It> join(It first, It last, const BasicCStringRef<char>& sep) {
  return ArgJoin<char, It>(first, last, sep);
}