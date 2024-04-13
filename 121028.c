unsigned lodepng_decode24(unsigned char** out, unsigned* w, unsigned* h, const unsigned char* in, size_t insize)
{
  return lodepng_decode_memory(out, w, h, in, insize, LCT_RGB, 8);
}