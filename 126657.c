static size_t packed_seek(void *fd, size_t offset) {
  struct packed_file *fp = (struct packed_file *) fd;
  fp->pos = offset;
  if (fp->pos > fp->size) fp->pos = fp->size;
  return fp->pos;
}