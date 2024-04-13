static int mbed_rng(void *ctx, unsigned char *buf, size_t len) {
  mg_random(buf, len);
  (void) ctx;
  return 0;
}