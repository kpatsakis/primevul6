static int multipart_buffer_eof(multipart_buffer *self) {
  if ( (self->bytes_in_buffer == 0 && fill_buffer(self) < 1) ) {
    return 1;
  } else {
    return 0;
  }
}