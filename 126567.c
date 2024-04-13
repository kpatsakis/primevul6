static struct mg_fd *p_open(const char *path, int flags) {
  const char *mode = flags == (MG_FS_READ | MG_FS_WRITE) ? "r+b"
                     : flags & MG_FS_READ                ? "rb"
                     : flags & MG_FS_WRITE               ? "wb"
                                                         : "";
  void *fp = NULL;
  struct mg_fd *fd = NULL;
#ifdef _WIN32
  wchar_t b1[PATH_MAX], b2[10];
  MultiByteToWideChar(CP_UTF8, 0, path, -1, b1, sizeof(b1) / sizeof(b1[0]));
  MultiByteToWideChar(CP_UTF8, 0, mode, -1, b2, sizeof(b2) / sizeof(b2[0]));
  fp = (void *) _wfopen(b1, b2);
#else
  fp = (void *) fopen(path, mode);
#endif
  if (fp == NULL) return NULL;
  fd = (struct mg_fd *) calloc(1, sizeof(*fd));
  fd->fd = fp;
  fd->fs = &mg_fs_posix;
  return fd;
}