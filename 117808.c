static void inbound_frame_set_mark(nghttp2_inbound_frame *iframe, size_t left) {
  nghttp2_buf_reset(&iframe->sbuf);
  iframe->sbuf.mark += left;
}