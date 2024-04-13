static FILE *openw(char *op) {
  PFV("Encoding %s ...", OP);
  if(!op) return stdout;
  FILE *fp;
#define EO(x) \
  if(!(x)) { \
    PF("ERROR opening %s for %s: %s", op, force ? "writing" : "creation", \
      strerror(errno)); \
    return 0; \
  }
#ifdef NOFOPENX
  int fd = open(op, O_WRONLY | O_CREAT | O_TRUNC | (!force * O_EXCL) | O_BINARY,
#ifdef _WIN32
    S_IREAD | S_IWRITE
#else
    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
#endif
  );
  EO(fd != -1)
  if(!(fp = fdopen(fd, "wb"))) {
    PF("ERROR opening %s for %s: %s", op, force ? "writing" : "creation",
      strerror(errno));
    close(fd);
    remove(op);
    return 0;
  }
#else
  EO(fp = fopen(op, force ? "wb" : "wbx"))
#endif
  return fp;
}