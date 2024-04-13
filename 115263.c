int32_t ByteArray::Put(int index, ByteVector* b) {
  assert(b);
  return Put(index, &((*b)[0]), 0, b->size());
}