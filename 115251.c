int32_t ByteArray::CopyTo(OutputStream* os, int32_t offset, int32_t length) {
  ByteVector b(COPY_BUFFER_SIZE);
  int32_t bytes_read = 0;
  int32_t index = 0;
  int32_t buffer_length = std::min<int32_t>(COPY_BUFFER_SIZE, length);
  while ((bytes_read = Get(index + offset, &(b[0]), 0, buffer_length)) > 0) {
    os->Write(&b, 0, bytes_read);
    index += bytes_read;
    buffer_length = std::min<int32_t>(b.size(), length - index);
  }
  return index;
}