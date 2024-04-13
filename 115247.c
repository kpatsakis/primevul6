int32_t ByteArray::Get(int32_t index, ByteVector* b) {
  assert(b);
  return Get(index, &((*b)[0]), 0, b->size());
}