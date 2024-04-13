bool ByteArray::CopyFrom(InputStream* is, int32_t length) {
  ByteVector b(COPY_BUFFER_SIZE);
  int32_t bytes_read = 0;
  int32_t index = 0;
  int32_t buffer_length = std::min<int32_t>(COPY_BUFFER_SIZE, length);
  while ((bytes_read = is->Read(&b, 0, buffer_length)) > 0) {
    if (Put(index, &(b[0]), 0, bytes_read) != bytes_read) {
#if defined (SFNTLY_NO_EXCEPTION)
      return 0;
#else
      throw IOException("Error writing bytes.");
#endif
    }
    index += bytes_read;
    length -= bytes_read;
    buffer_length = std::min<int32_t>(b.size(), length);
  }
  return true;
}