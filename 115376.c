template <typename T> const T &any_cast(const any &val) {
  assert(val.content_);
  auto p = dynamic_cast<any::holder<T> *>(val.content_);
  assert(p);
  if (!p) { throw std::bad_cast(); }
  return p->value_;
}