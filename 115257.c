int32_t ByteArray::CopyTo(ByteArray* array, int32_t offset, int32_t length) {
  return CopyTo(0, array, offset, length);
}