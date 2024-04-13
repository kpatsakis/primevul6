int32_t ByteArray::Get(int32_t index,
                       byte_t* b,
                       int32_t offset,
                       int32_t length) {
  assert(b);
  if (index < 0 || index >= filled_length_) {
    return 0;
  }
  int32_t actual_length = std::min<int32_t>(length, filled_length_ - index);
  return InternalGet(index, b, offset, actual_length);
}