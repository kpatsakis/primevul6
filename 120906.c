static size_t lodepng_get_raw_size_idat(unsigned w, unsigned h, const LodePNGColorMode* color)
{
  return h * ((w * lodepng_get_bpp(color) + 7) / 8);
}