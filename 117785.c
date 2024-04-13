static ssize_t inbound_frame_compute_pad(nghttp2_inbound_frame *iframe) {
  size_t padlen;

  /* 1 for Pad Length field */
  padlen = (size_t)(iframe->sbuf.pos[0] + 1);

  DEBUGF("recv: padlen=%zu\n", padlen);

  /* We cannot use iframe->frame.hd.length because of CONTINUATION */
  if (padlen - 1 > iframe->payloadleft) {
    return -1;
  }

  iframe->padlen = padlen;

  return (ssize_t)padlen;
}