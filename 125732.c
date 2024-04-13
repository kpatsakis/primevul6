static void pngwerr(png_struct *p, const char *s) {
  PF("ERROR writing %s: %s", (char *)png_get_error_ptr(p), s);
  png_longjmp(p, 1);
}