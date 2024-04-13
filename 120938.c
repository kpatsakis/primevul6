unsigned lodepng_color_mode_copy(LodePNGColorMode* dest, const LodePNGColorMode* source)
{
  size_t i;
  lodepng_color_mode_cleanup(dest);
  *dest = *source;
  if(source->palette)
  {
    dest->palette = (unsigned char*)malloc(1024);
    if(!dest->palette && source->palettesize) return 83; /*alloc fail*/
    for(i = 0; i < source->palettesize * 4; i++) dest->palette[i] = source->palette[i];
  }
  return 0;
}