static size_t ws_process(uint8_t *buf, size_t len, struct ws_msg *msg) {
  size_t i, n = 0, mask_len = 0;
  memset(msg, 0, sizeof(*msg));
  if (len >= 2) {
    n = buf[1] & 0x7f;                // Frame length
    mask_len = buf[1] & 128 ? 4 : 0;  // last bit is a mask bit
    msg->flags = buf[0];
    if (n < 126 && len >= mask_len) {
      msg->data_len = n;
      msg->header_len = 2 + mask_len;
    } else if (n == 126 && len >= 4 + mask_len) {
      msg->header_len = 4 + mask_len;
      msg->data_len = mg_ntohs(*(uint16_t *) &buf[2]);
    } else if (len >= 10 + mask_len) {
      msg->header_len = 10 + mask_len;
      msg->data_len =
          (size_t) (((uint64_t) mg_ntohl(*(uint32_t *) &buf[2])) << 32) +
          mg_ntohl(*(uint32_t *) &buf[6]);
    }
  }
  // Sanity check, and integer overflow protection for the boundary check below
  // data_len should not be larger than 1 Gb
  if (msg->data_len > 1024 * 1024 * 1024) return 0;
  if (msg->header_len + msg->data_len > len) return 0;
  if (mask_len > 0) {
    uint8_t *p = buf + msg->header_len, *m = p - mask_len;
    for (i = 0; i < msg->data_len; i++) p[i] ^= m[i & 3];
  }
  return msg->header_len + msg->data_len;
}