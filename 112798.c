static multipart_buffer *multipart_buffer_new(Transport *transport,
                                              const char *data, size_t size,
                                              std::string boundary) {
  multipart_buffer *self =
    (multipart_buffer *)calloc(1, sizeof(multipart_buffer));

  self->transport = transport;
  auto minsize = boundary.length() + 6;
  if (minsize < FILLUNIT) minsize = FILLUNIT;

  self->buffer = (char *) calloc(1, minsize + 1);
  self->bufsize = minsize;

  vspprintf(&self->boundary, 0, "--%s", boundary.c_str());

  self->boundary_next_len =
    vspprintf(&self->boundary_next, 0, "\n--%s", boundary.c_str());

  self->buf_begin = self->buffer;
  self->bytes_in_buffer = 0;

  self->post_data = data;
  self->cursor = (char*)self->post_data;
  self->post_size = size;
  self->throw_size = 0;
  return self;
}