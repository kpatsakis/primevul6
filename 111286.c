string TensorSlice::DebugString() const {
  string buffer;
  bool first = true;
  for (int d = 0; d < dims(); ++d) {
    if (!first) {
      buffer.append(":");
    }
    if (IsFullAt(d)) {
      buffer.append("-");
    } else {
      strings::StrAppend(&buffer, starts_[d], ",", lengths_[d]);
    }
    first = false;
  }
  return buffer;
}