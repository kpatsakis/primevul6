static char *get_line(multipart_buffer *self) {
  char* ptr = next_line(self);

  if (!ptr) {
    fill_buffer(self);
    ptr = next_line(self);
  }

  return ptr;
}