static void pngread(png_struct *p, uint8_t *d, size_t s) {
  if(!fread(d, s, 1, png_get_io_ptr(p))) png_error(p, "I/O error");
  pnglen += s;
}