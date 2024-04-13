static void packed_close(struct mg_fd *fd) {
  if (fd) free(fd->fd), free(fd);
}