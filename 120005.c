static MagickBooleanType ReadUncompressedRGBA(Image *image, DDSInfo *dds_info,
  ExceptionInfo *exception)
{
  register Quantum
    *q;

  ssize_t
    alphaBits,
    x,
    y;

  unsigned short
    color;

  alphaBits=0;
  if (dds_info->pixelformat.rgb_bitcount == 16)
    {
      if (IsBitMask(dds_info->pixelformat,0x7c00,0x03e0,0x001f,0x8000))
        alphaBits=1;
      else if (IsBitMask(dds_info->pixelformat,0x00ff,0x00ff,0x00ff,0xff00))
        {
          alphaBits=2;
          (void) SetImageType(image,GrayscaleAlphaType,exception);
        }
      else if (IsBitMask(dds_info->pixelformat,0x0f00,0x00f0,0x000f,0xf000))
        alphaBits=4;
      else
        ThrowBinaryException(CorruptImageError,"ImageTypeNotSupported",
          image->filename);
    }

  for (y = 0; y < (ssize_t) dds_info->height; y++)
  {
    q = QueueAuthenticPixels(image, 0, y, dds_info->width, 1,exception);
    
    if (q == (Quantum *) NULL)
      return MagickFalse;
    
    for (x = 0; x < (ssize_t) dds_info->width; x++)
    {
      if (dds_info->pixelformat.rgb_bitcount == 16)
        {
           color=ReadBlobShort(image);
           if (alphaBits == 1)
             {
               SetPixelAlpha(image,(color & (1 << 15)) ? QuantumRange : 0,q);
               SetPixelRed(image,ScaleCharToQuantum((unsigned char)
                 ((((unsigned short)(color << 1) >> 11)/31.0)*255)),q);
               SetPixelGreen(image,ScaleCharToQuantum((unsigned char)
                 ((((unsigned short)(color << 6) >> 11)/31.0)*255)),q);
               SetPixelBlue(image,ScaleCharToQuantum((unsigned char)
                 ((((unsigned short)(color << 11) >> 11)/31.0)*255)),q);
             }
          else if (alphaBits == 2)
            {
               SetPixelAlpha(image,ScaleCharToQuantum((unsigned char)
                 (color >> 8)),q);
               SetPixelGray(image,ScaleCharToQuantum((unsigned char)color),q);
            }
          else
            {
               SetPixelAlpha(image,ScaleCharToQuantum((unsigned char)
                 (((color >> 12)/15.0)*255)),q);
               SetPixelRed(image,ScaleCharToQuantum((unsigned char)
                 ((((unsigned short)(color << 4) >> 12)/15.0)*255)),q);
               SetPixelGreen(image,ScaleCharToQuantum((unsigned char)
                 ((((unsigned short)(color << 8) >> 12)/15.0)*255)),q);
               SetPixelBlue(image,ScaleCharToQuantum((unsigned char)
                 ((((unsigned short)(color << 12) >> 12)/15.0)*255)),q);
            }
        }
      else
        {
          SetPixelBlue(image,ScaleCharToQuantum((unsigned char)
            ReadBlobByte(image)),q);
          SetPixelGreen(image,ScaleCharToQuantum((unsigned char)
            ReadBlobByte(image)),q);
          SetPixelRed(image,ScaleCharToQuantum((unsigned char)
            ReadBlobByte(image)),q);
          SetPixelAlpha(image,ScaleCharToQuantum((unsigned char)
            ReadBlobByte(image)),q);
        }
      q+=GetPixelChannels(image);
    }
    
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      return MagickFalse;
  }
  
  return(SkipRGBMipmaps(image,dds_info,4,exception));
}