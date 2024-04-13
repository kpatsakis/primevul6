static int webpwrite(const uint8_t *d, size_t s, const WebPPicture *p) {
  return (int)fwrite(d, s, 1, p->custom_ptr);
}