template <typename T> T &any_cast(any &val) {
  if (!val.content_) { throw std::bad_cast(); }
  auto p = dynamic_cast<any::holder<T> *>(val.content_);
  assert(p);
  if (!p) { throw std::bad_cast(); }
  return p->value_;
}