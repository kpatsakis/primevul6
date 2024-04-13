static int recv_fd(int afdt_fd) {
  int fd;
  afdt_error_t err;
  uint8_t afdt_buf[AFDT_MSGLEN];
  uint32_t afdt_len;
  errno = 0;
  if (afdt_recv_fd_msg(afdt_fd, afdt_buf, &afdt_len, &fd, &err) < 0) {
    if (errno == 0) {
      // Set non-empty errno if afdt_send_fd_msg doesn't set one on error
      errno = EPROTO;
    }
    return -1;
  }
  return fd;
}