void lodepng_palette_clear(LodePNGColorMode* info)
{
  free(info->palette);
  info->palette = 0;
  info->palettesize = 0;
}