static void zeromem(volatile unsigned char *buf, size_t len) {
  if (buf != NULL) {
    while (len--) *buf++ = 0;
  }
}