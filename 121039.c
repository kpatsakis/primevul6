static unsigned getPaletteTranslucency(const unsigned char* palette, size_t palettesize)
{
  size_t i;
  unsigned key = 0;
  unsigned r = 0, g = 0, b = 0; /*the value of the color with alpha 0, so long as color keying is possible*/
  for(i = 0; i < palettesize; i++)
  {
    if(!key && palette[4 * i + 3] == 0)
    {
      r = palette[4 * i + 0]; g = palette[4 * i + 1]; b = palette[4 * i + 2];
      key = 1;
      i = (size_t)(-1); /*restart from beginning, to detect earlier opaque colors with key's value*/
    }
    else if(palette[4 * i + 3] != 255) return 2;
    /*when key, no opaque RGB may have key's RGB*/
    else if(key && r == palette[i * 4 + 0] && g == palette[i * 4 + 1] && b == palette[i * 4 + 2]) return 2;
  }
  return key;
}