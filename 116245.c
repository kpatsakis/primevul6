auto join(const Range& range, const BasicCStringRef<char>& sep)
    -> ArgJoin<char, decltype(std::begin(range))> {
  return join(std::begin(range), std::end(range), sep);
}