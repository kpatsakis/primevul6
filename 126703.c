void mg_iobuf_free(struct mg_iobuf *io) {
  mg_iobuf_resize(io, 0);
}