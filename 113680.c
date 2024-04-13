static bool send_fd(int afdt_fd, int fd) {
  afdt_error_t err;
  errno = 0;
  int ret = afdt_send_fd_msg(afdt_fd, 0, 0, fd, &err);
  if (ret < 0 && errno == 0) {
    // Set non-empty errno if afdt_send_fd_msg doesn't set one on error
    errno = EPROTO;
  }
  return ret >= 0;
}