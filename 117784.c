static size_t inbound_frame_payload_readlen(nghttp2_inbound_frame *iframe,
                                            const uint8_t *in,
                                            const uint8_t *last) {
  return nghttp2_min((size_t)(last - in), iframe->payloadleft);
}