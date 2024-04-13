static bool p2w(char *ip, char *op) {
  FILE *fp = openr(ip);
  if(!fp) return 1;
  uint32_t *b = 0;
  png_info *n = 0;
  char *k[] = {"Out of memory",
    "???", // oom flushing bitstream, unused in libwebp
    "???", // null param
    "Broken config, file a bug report",
    "???", // image too big (already checked)
    "???", "???", // lossy
    "I/O error",
    "???", // lossy
    "???"}; // canceled
  png_struct *p =
    png_create_read_struct(PNG_LIBPNG_VER_STRING, ip, pngrerr, pngwarn);
  if(!p) {
    PF("ERROR reading %s: %s", IP, *k);
    goto p2w_close;
  }
  n = png_create_info_struct(p);
  if(!n) {
    PF("ERROR reading %s: %s", IP, *k);
    goto p2w_close;
  }
  if(setjmp(png_jmpbuf(p))) {
  p2w_close:
    fclose(fp);
    png_destroy_read_struct(&p, &n, 0);
  p2w_free:
    free(b);
    return 1;
  }
  pnglen = 0;
  png_set_read_fn(p, fp, pngread);
  png_read_info(p, n);
  uint32_t width, height;
  int bitdepth, colortype;
  png_get_IHDR(p, n, &width, &height, &bitdepth, &colortype, 0, 0, 0);
  if(width > 16383 || height > 16383) {
    PF("ERROR reading %s: Image too big (%" PRIu32 " x %" PRIu32
       ", max. 16383 x 16383 px)",
      IP, width, height);
    goto p2w_close;
  }
  if((unsigned)bitdepth > 8)
    PF("Warning: %s is 16-bit, will be downsampled to 8-bit", IP);
  bool trns = png_get_valid(p, n, PNG_INFO_tRNS);
#ifdef FIXEDGAMMA
#define GAMMA ((uint32_t)gamma) / 1e5
  int32_t gamma = 45455;
  if(png_get_valid(p, n, PNG_INFO_sRGB) || png_get_gAMA_fixed(p, n, &gamma))
    png_set_gamma_fixed(p, 22e4, gamma);
#else
#define GAMMA gamma
  double gamma = 1 / 2.2;
  if(png_get_valid(p, n, PNG_INFO_sRGB) || png_get_gAMA(p, n, &gamma))
    png_set_gamma(p, 2.2, gamma);
#endif
#define S(x) png_set_##x(p)
  S(scale_16);
  S(expand);
  S(gray_to_rgb);
  S(packing);
  if(*(uint8_t *)&(uint16_t){1}) {
    S(bgr);
    png_set_add_alpha(p, 255, PNG_FILLER_AFTER);
  } else {
    // TODO: test somehow
    S(swap_alpha);
    png_set_add_alpha(p, 255, PNG_FILLER_BEFORE);
  }
  int passes = S(interlace_handling);
  png_read_update_info(p, n);
#ifndef NDEBUG
  size_t rowbytes = png_get_rowbytes(p, n);
  if(rowbytes != (size_t)4 * width) {
    PF("ERROR reading %s: rowbytes is %zu, should be %zu", IP, rowbytes,
      (size_t)4 * width);
    goto p2w_close;
  }
#endif
  b = malloc(width * height * 4);
  if(!b) {
    PF("ERROR reading %s: %s", IP, *k);
    goto p2w_close;
  }
  for(unsigned x = (unsigned)passes; x; x--) {
    uint8_t *w = (uint8_t *)b;
    for(unsigned y = height; y; y--) {
      png_read_row(p, w, 0);
      w += width * 4;
    }
  }
  png_read_end(p, 0);
  png_destroy_read_struct(&p, &n, 0);
  fclose(fp);
  char *f[] = {
    "greyscale", "???", "RGB", "paletted", "greyscale + alpha", "???", "RGBA"};
  PFV("Info: %s:\nDimensions: %" PRIu32 " x %" PRIu32
      "\nSize: %zu bytes (%.15g bpp)\nFormat: %u-bit %s%s%s\nGamma: %.5g",
    IP, width, height, pnglen, (double)pnglen * 8 / (width * height), bitdepth,
    f[(unsigned)colortype], trns ? ", with transparency" : "",
    (unsigned)passes > 1 ? ", interlaced" : "", GAMMA);
  WebPConfig c;
  if(!WebPConfigPreset(&c, WEBP_PRESET_ICON, 100)) {
    PF("ERROR writing %s: %s", OP, k[3]);
    goto p2w_free;
  }
  if(!(fp = openw(op))) goto p2w_free;
  c.lossless = 1;
  c.method = 6;
#ifndef NOTHREADS
  c.thread_level = 1; // doesn't seem to affect output
#endif
  c.exact = exact;
  WebPAuxStats s;
  WebPPicture o = {1, .width = (int)width, (int)height, .argb = b,
    .argb_stride = (int)width, .writer = webpwrite, .custom_ptr = fp,
    .stats = verbose ? &s : 0};
  // progress_hook only reports 1, 5, 90, 100 for lossless
  trns = (trns || (colortype & PNG_COLOR_MASK_ALPHA)) &&
    WebPPictureHasTransparency(&o);
  if(!WebPEncode(&c, &o)) {
    PF("ERROR writing %s: %s", OP, k[o.error_code - 1]);
    fclose(fp);
  p2w_rm:
    if(op) remove(op);
    goto p2w_free;
  }
  if(fclose(fp)) {
    PF("ERROR closing %s: %s", OP, strerror(errno));
    goto p2w_rm;
  }
  free(b);
#define F s.lossless_features
#define C s.palette_size
  PFV("Info: %s:\nDimensions: %u x %u\nSize: %u bytes (%.15g bpp)\n\
Header size: %u, image data size: %u\nUses alpha: %s\n\
Precision bits: histogram=%u transform=%u cache=%u\n\
Lossless features:%s%s%s%s\nColors: %s%u",
    OP, o.width, o.height, s.lossless_size,
    (unsigned)s.lossless_size * 8. / (unsigned)(o.width * o.height),
    s.lossless_hdr_size, s.lossless_data_size, trns ? "yes" : "no",
    s.histogram_bits, s.transform_bits, s.cache_bits,
    F ? F & 1 ? " prediction" : "" : " none", F && F & 2 ? " cross-color" : "",
    F && F & 4 ? " subtract-green" : "", F && F & 8 ? " palette" : "",
    C ? "" : ">", C ? C : 256);
  return 0;
}