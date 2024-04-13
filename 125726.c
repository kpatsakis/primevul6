static void pngwarn(png_struct *p, const char *s) {
  PF("Warning: %s: %s", (char *)png_get_error_ptr(p), s);
}