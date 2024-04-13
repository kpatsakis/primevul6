int32_t ByteArray::CopyTo(int32_t dst_offset, ByteArray* array,
                          int32_t src_offset, int32_t length) {
  assert(array);
  if (array->Size() < dst_offset + length) {  // insufficient space
    return -1;
  }

  ByteVector b(COPY_BUFFER_SIZE);
  int32_t bytes_read = 0;
  int32_t index = 0;
  int32_t remaining_length = length;
  int32_t buffer_length = std::min<int32_t>(COPY_BUFFER_SIZE, length);
  while ((bytes_read =
              Get(index + src_offset, &(b[0]), 0, buffer_length)) > 0) {
    int bytes_written = array->Put(index + dst_offset, &(b[0]), 0, bytes_read);
    UNREFERENCED_PARAMETER(bytes_written);
    index += bytes_read;
    remaining_length -= bytes_read;
    buffer_length = std::min<int32_t>(b.size(), remaining_length);
  }
  return index;
}