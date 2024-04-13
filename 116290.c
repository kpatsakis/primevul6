auto join(const Range& range, const BasicCStringRef<wchar_t>& sep)
    -> ArgJoin<wchar_t, decltype(std::begin(range))> {
  return join(std::begin(range), std::end(range), sep);
}