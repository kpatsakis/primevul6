Status TensorSlice::Parse(const string& str, TensorSlice* slice) {
  std::vector<string> items = str_util::Split(str, ':', str_util::SkipEmpty());
  slice->starts_.reserve(items.size());
  slice->lengths_.reserve(items.size());
  for (const string& x : items) {
    int64_t s, l;
    if (x == "-") {
      // "everything"
      s = 0;
      l = kFullExtent;
    } else {
      std::vector<string> sl = str_util::Split(x, ',', str_util::SkipEmpty());
      if (sl.size() != 2 || !strings::safe_strto64(sl[0], &s) ||
          !strings::safe_strto64(sl[1], &l)) {
        return errors::InvalidArgument(
            "Expected a pair of numbers or '-' "
            "but got '",
            x, "': string = ", str);
      }
      if (s < 0 || l <= 0) {
        return errors::InvalidArgument(
            "Expected non-negative start and "
            "positive length but got start = ",
            s, ", length = ", l, ": string = ", str);
      }
    }
    slice->starts_.push_back(s);
    slice->lengths_.push_back(l);
  }

  return Status::OK();
}