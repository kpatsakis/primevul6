static void restore_http_cb(struct mg_connection *c) {
  struct mg_fd *fd = (struct mg_fd *) c->pfn_data;
  if (fd != NULL) fd->fs->close(fd);
  c->pfn_data = NULL;
  c->pfn = http_cb;
}