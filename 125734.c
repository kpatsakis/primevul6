static FILE *openr(char *ip) {
  PFV("Decoding %s ...", IP);
  if(!ip) return stdin;
  FILE *fp;
#ifdef NOFOPENX
  int fd = open(ip, O_RDONLY | O_BINARY);
  if(fd == -1) {
    PF("ERROR opening %s for %s: %s", ip, "reading", strerror(errno));
    return 0;
  }
  if(!(fp = fdopen(fd, "rb"))) {
    PF("ERROR opening %s for %s: %s", ip, "reading", strerror(errno));
    close(fd);
    return 0;
  }
#else
  if(!(fp = fopen(ip, "rb"))) {
    PF("ERROR opening %s for %s: %s", ip, "reading", strerror(errno));
    return 0;
  }
#endif
  return fp;
}