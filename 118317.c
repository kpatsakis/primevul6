int32_t FontData::BoundLength(int32_t offset, int32_t length) {
  return std::min<int32_t>(length, bound_length_ - offset);
}