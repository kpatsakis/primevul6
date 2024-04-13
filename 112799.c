static uint32_t read_post(multipart_buffer *self, char *buf,
                          uint32_t bytes_to_read) {
  always_assert(bytes_to_read > 0);
  always_assert(self->post_data);
  always_assert(self->cursor >= self->post_data);
  int64_t bytes_remaining = (self->post_size - self->throw_size) -
                            (self->cursor - self->post_data);
  always_assert(bytes_remaining >= 0);
  if (bytes_to_read <= bytes_remaining) {
    memcpy(buf, self->cursor, bytes_to_read);
    self->cursor += bytes_to_read;
    return bytes_to_read;
  }

  uint32_t bytes_read = bytes_remaining;
  memcpy(buf, self->cursor, bytes_remaining);
  bytes_to_read -= bytes_remaining;
  self->cursor += bytes_remaining;
  always_assert(self->cursor == (char *)self->post_data +
                        (self->post_size - self->throw_size));
  while (bytes_to_read > 0 && self->transport->hasMorePostData()) {
    size_t extra_byte_read = 0;
    const void *extra = self->transport->getMorePostData(extra_byte_read);
    if (extra_byte_read == 0) break;
    if (RuntimeOption::AlwaysPopulateRawPostData) {
      // Possible overflow in buffer_append if post_size + extra_byte_read >=
      // MAX INT
      self->post_data = (const char *)buffer_append(
        self->post_data, self->post_size, extra, extra_byte_read);
      self->cursor = (char*)self->post_data + self->post_size;
    } else {
      self->post_data = (const char *)extra;
      self->throw_size = self->post_size;
      self->cursor = (char*)self->post_data;
    }
    self->post_size += extra_byte_read;
    if (bytes_to_read <= extra_byte_read) {
      memcpy(buf + bytes_read, self->cursor, bytes_to_read);
      self->cursor += bytes_to_read;
      return bytes_read + bytes_to_read;
    }
    memcpy(buf + bytes_read, self->cursor, extra_byte_read);
    bytes_to_read -= extra_byte_read;
    bytes_read += extra_byte_read;
    self->cursor += extra_byte_read;
  }
  return bytes_read;
}