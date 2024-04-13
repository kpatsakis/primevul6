int mg_iobuf_init(struct mg_iobuf *io, size_t size) {
  return mg_iobuf_resize(io, size);
}