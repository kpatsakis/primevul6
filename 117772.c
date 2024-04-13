static int adjust_recv_window_size(int32_t *recv_window_size_ptr, size_t delta,
                                   int32_t local_window_size) {
  if (*recv_window_size_ptr > local_window_size - (int32_t)delta ||
      *recv_window_size_ptr > NGHTTP2_MAX_WINDOW_SIZE - (int32_t)delta) {
    return -1;
  }
  *recv_window_size_ptr += (int32_t)delta;
  return 0;
}