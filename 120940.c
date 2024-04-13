void lodepng_color_mode_init(LodePNGColorMode* info)
{
  info->key_defined = 0;
  info->key_r = info->key_g = info->key_b = 0;
  info->colortype = LCT_RGBA;
  info->bitdepth = 8;
  info->palette = 0;
  info->palettesize = 0;
}