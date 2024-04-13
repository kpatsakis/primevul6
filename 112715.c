Variant StringUtil::Split(const String& str, int64_t split_length /* = 1 */) {
  if (split_length <= 0) {
    throw_invalid_argument(
      "The length of each segment must be greater than zero"
    );
    return false;
  }

  int len = str.size();
  PackedArrayInit ret(len / split_length + 1, CheckAllocation{});
  if (split_length >= len) {
    ret.append(str);
  } else {
    for (int i = 0; i < len; i += split_length) {
      ret.append(str.substr(i, split_length));
    }
  }
  return ret.toArray();
}