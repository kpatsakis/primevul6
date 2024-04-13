int32_t ByteArray::Put(int32_t index,
                       byte_t* b,
                       int32_t offset,
                       int32_t length) {
  assert(b);
  if (index < 0 || index >= Size()) {
#if defined (SFNTLY_NO_EXCEPTION)
    return 0;
#else
    throw IndexOutOfBoundException(
        "Attempt to write outside the bounds of the data");
#endif
  }
  int32_t actual_length = std::min<int32_t>(length, Size() - index);
  int32_t bytes_written = InternalPut(index, b, offset, actual_length);
  filled_length_ = std::max<int32_t>(filled_length_, index + bytes_written);
  return bytes_written;
}