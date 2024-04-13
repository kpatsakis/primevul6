static struct mg_fd *packed_open(const char *path, int flags) {
  size_t size = 0;
  const char *data = mg_unpack(path, &size, NULL);
  struct packed_file *fp = NULL;
  struct mg_fd *fd = NULL;
  if (data == NULL) return NULL;
  if (flags & MG_FS_WRITE) return NULL;
  fp = (struct packed_file *) calloc(1, sizeof(*fp));
  fd = (struct mg_fd *) calloc(1, sizeof(*fd));
  fp->size = size;
  fp->data = data;
  fd->fd = fp;
  fd->fs = &mg_fs_packed;
  return fd;
}