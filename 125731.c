static void pngrerr(png_struct *p, const char *s) {
  PF("ERROR reading %s: %s", (char *)png_get_error_ptr(p), s);
  png_longjmp(p, 1);
}