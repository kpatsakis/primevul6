static int lodepng_color_mode_equal(const LodePNGColorMode* a, const LodePNGColorMode* b)
{
  size_t i;
  if(a->colortype != b->colortype) return 0;
  if(a->bitdepth != b->bitdepth) return 0;
  if(a->key_defined != b->key_defined) return 0;
  if(a->key_defined)
  {
    if(a->key_r != b->key_r) return 0;
    if(a->key_g != b->key_g) return 0;
    if(a->key_b != b->key_b) return 0;
  }
  if(a->palettesize != b->palettesize) return 0;
  for(i = 0; i < a->palettesize * 4; i++)
  {
    if(a->palette[i] != b->palette[i]) return 0;
  }
  return 1;
}