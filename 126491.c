void mg_random(void *buf, size_t len) {
  bool done = false;
  unsigned char *p = (unsigned char *) buf;
#if MG_ARCH == MG_ARCH_ESP32
  while (len--) *p++ = (unsigned char) (esp_random() & 255);
  done = true;
#elif MG_ARCH == MG_ARCH_WIN32
#elif MG_ARCH == MG_ARCH_UNIX
  FILE *fp = fopen("/dev/urandom", "rb");
  if (fp != NULL) {
    if (fread(buf, 1, len, fp) == len) done = true;
    fclose(fp);
  }
#endif
  // If everything above did not work, fallback to a pseudo random generator
  while (!done && len--) *p++ = (unsigned char) (rand() & 255);
}