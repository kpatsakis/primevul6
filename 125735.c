static void pngwrite(png_struct *p, uint8_t *d, size_t s) {
  if(!fwrite(d, s, 1, png_get_io_ptr(p))) png_error(p, "I/O error");
  pnglen += s;
}