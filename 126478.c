size_t mg_iobuf_del(struct mg_iobuf *io, size_t ofs, size_t len) {
  if (ofs > io->len) ofs = io->len;
  if (ofs + len > io->len) len = io->len - ofs;
  memmove(io->buf + ofs, io->buf + ofs + len, io->len - ofs - len);
  zeromem(io->buf + io->len - len, len);
  io->len -= len;
  return len;
}