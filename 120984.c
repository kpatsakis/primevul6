unsigned lodepng_is_alpha_type(const LodePNGColorMode* info)
{
  return (info->colortype & 4) != 0; /*4 or 6*/
}