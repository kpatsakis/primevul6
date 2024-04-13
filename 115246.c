int32_t ByteArray::CopyTo(ByteArray* array) {
  return CopyTo(array, 0, Length());
}