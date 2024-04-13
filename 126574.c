size_t mg_iobuf_add(struct mg_iobuf *io, size_t ofs, const void *buf,
                    size_t len, size_t chunk_size) {
  size_t new_size = io->len + len;
  if (new_size > io->size) {
    new_size += chunk_size;             // Make sure that io->size
    new_size -= new_size % chunk_size;  // is aligned by chunk_size boundary
    mg_iobuf_resize(io, new_size);      // Attempt to realloc
    if (new_size != io->size) len = 0;  // Realloc failure, append nothing
  }
  if (ofs < io->len) memmove(io->buf + ofs + len, io->buf + ofs, io->len - ofs);
  if (buf != NULL) memmove(io->buf + ofs, buf, len);
  if (ofs > io->len) io->len += ofs - io->len;
  io->len += len;
  return len;
}