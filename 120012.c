static MagickBooleanType SetDXT1Pixels(Image *image,ssize_t x,ssize_t y,
  DDSColors colors,size_t bits,Quantum *q)
{
  register ssize_t
    i;

  ssize_t
    j;

  unsigned char
    code;

  for (j = 0; j < 4; j++)
  {
    for (i = 0; i < 4; i++)
    {
      if ((x + i) < (ssize_t) image->columns &&
          (y + j) < (ssize_t) image->rows)
        {
          code=(unsigned char) ((bits >> ((j*4+i)*2)) & 0x3);
          SetPixelRed(image,ScaleCharToQuantum(colors.r[code]),q);
          SetPixelGreen(image,ScaleCharToQuantum(colors.g[code]),q);
          SetPixelBlue(image,ScaleCharToQuantum(colors.b[code]),q);
          SetPixelOpacity(image,ScaleCharToQuantum(colors.a[code]),q);
          if ((colors.a[code] != 0) &&
              (image->alpha_trait == UndefinedPixelTrait))
            return(MagickFalse);
          q+=GetPixelChannels(image);
        }
    }
  }
  return(MagickTrue);
}