void ByteArray::Put(int32_t index, byte_t b) {
  if (index < 0 || index >= Size()) {
#if defined (SFNTLY_NO_EXCEPTION)
    return;
#else
    throw IndexOutOfBoundException(
        "Attempt to write outside the bounds of the data");
#endif
  }
  InternalPut(index, b);
  filled_length_ = std::max<int32_t>(filled_length_, index + 1);
}