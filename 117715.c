static size_t inbound_frame_buf_read(nghttp2_inbound_frame *iframe,
                                     const uint8_t *in, const uint8_t *last) {
  size_t readlen;

  readlen =
      nghttp2_min((size_t)(last - in), nghttp2_buf_mark_avail(&iframe->sbuf));

  iframe->sbuf.last = nghttp2_cpymem(iframe->sbuf.last, in, readlen);

  return readlen;
}