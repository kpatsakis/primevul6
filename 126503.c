static void test_iobuf(void) {
  struct mg_iobuf io = {0, 0, 0};
  ASSERT(io.buf == NULL && io.size == 0 && io.len == 0);
  mg_iobuf_resize(&io, 1);
  ASSERT(io.buf != NULL && io.size == 1 && io.len == 0);
  ASSERT(memcmp(io.buf, "\x00", 1) == 0);
  mg_iobuf_add(&io, 3, "hi", 2, 10);
  ASSERT(io.buf != NULL && io.size == 10 && io.len == 5);
  ASSERT(memcmp(io.buf, "\x00\x00\x00hi", 5) == 0);
  mg_iobuf_add(&io, io.len, "!", 1, 10);
  ASSERT(io.buf != NULL && io.size == 10 && io.len == 6);
  ASSERT(memcmp(io.buf, "\x00\x00\x00hi!", 6) == 0);
  mg_iobuf_add(&io, 0, "x", 1, 10);
  ASSERT(memcmp(io.buf, "x\x00\x00\x00hi!", 7) == 0);
  ASSERT(io.buf != NULL && io.size == 10 && io.len == 7);
  mg_iobuf_del(&io, 1, 3);
  ASSERT(io.buf != NULL && io.size == 10 && io.len == 4);
  ASSERT(memcmp(io.buf, "xhi!", 3) == 0);
  mg_iobuf_del(&io, 10, 100);
  ASSERT(io.buf != NULL && io.size == 10 && io.len == 4);
  ASSERT(memcmp(io.buf, "xhi!", 3) == 0);
  free(io.buf);
}