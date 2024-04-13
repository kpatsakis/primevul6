unsigned lodepng_is_palette_type(const LodePNGColorMode* info)
{
  return info->colortype == LCT_PALETTE;
}