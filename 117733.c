static ssize_t inbound_frame_effective_readlen(nghttp2_inbound_frame *iframe,
                                               size_t payloadleft,
                                               size_t readlen) {
  size_t trail_padlen =
      nghttp2_frame_trail_padlen(&iframe->frame, iframe->padlen);

  if (trail_padlen > payloadleft) {
    size_t padlen;
    padlen = trail_padlen - payloadleft;
    if (readlen < padlen) {
      return -1;
    }
    return (ssize_t)(readlen - padlen);
  }
  return (ssize_t)(readlen);
}