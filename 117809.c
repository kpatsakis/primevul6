static int inbound_frame_handle_pad(nghttp2_inbound_frame *iframe,
                                    nghttp2_frame_hd *hd) {
  if (hd->flags & NGHTTP2_FLAG_PADDED) {
    if (hd->length < 1) {
      return -1;
    }
    inbound_frame_set_mark(iframe, 1);
    return 1;
  }
  DEBUGF("recv: no padding in payload\n");
  return 0;
}