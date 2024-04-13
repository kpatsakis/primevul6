static void pngflush(png_struct *p) {
#ifdef DOFLUSH
  fflush(png_get_io_ptr(p));
#else
  (void)p;
#endif
}