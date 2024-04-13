int mg_iobuf_resize(struct mg_iobuf *io, size_t new_size) {
  int ok = 1;
  if (new_size == 0) {
    zeromem(io->buf, io->size);
    free(io->buf);
    io->buf = NULL;
    io->len = io->size = 0;
  } else if (new_size != io->size) {
    // NOTE(lsm): do not use realloc here. Use calloc/free only, to ease the
    // porting to some obscure platforms like FreeRTOS
    void *p = calloc(1, new_size);
    if (p != NULL) {
      size_t len = new_size < io->len ? new_size : io->len;
      if (len > 0) memcpy(p, io->buf, len);
      zeromem(io->buf, io->size);
      free(io->buf);
      io->buf = (unsigned char *) p;
      io->size = new_size;
    } else {
      ok = 0;
      LOG(LL_ERROR,
          ("%lu->%lu", (unsigned long) io->size, (unsigned long) new_size));
    }
  }
  return ok;
}