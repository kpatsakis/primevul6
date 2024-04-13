static void p_close(struct mg_fd *fd) {
  if (fd != NULL) fclose((FILE *) fd->fd), free(fd);
}