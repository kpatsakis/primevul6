static MagickBooleanType ReadDXT1(Image *image,DDSInfo *dds_info,
  ExceptionInfo *exception)
{
  DDSColors
    colors;

  register Quantum
    *q;

  register ssize_t
    x;

  size_t
    bits;

  ssize_t
    y;

  unsigned short
    c0,
    c1;

  for (y = 0; y < (ssize_t) image->rows; y += 4)
  {
    for (x = 0; x < (ssize_t) image->columns; x += 4)
    {
      /* Get 4x4 patch of pixels to write on */
      q=QueueAuthenticPixels(image,x,y,MagickMin(4,image->columns-x),
        MagickMin(4,image->rows-y),exception);

      if (q == (Quantum *) NULL)
        return MagickFalse;

      /* Read 8 bytes of data from the image */
      c0=ReadBlobLSBShort(image);
      c1=ReadBlobLSBShort(image);
      bits=ReadBlobLSBLong(image);

      CalculateColors(c0,c1,&colors,MagickFalse);

      /* Write the pixels */
      if (SetDXT1Pixels(image,x,y,colors,bits,q) == MagickFalse)
        {
          /* Correct alpha */
          SetImageAlpha(image,QuantumRange,exception);
          q=QueueAuthenticPixels(image,x,y,MagickMin(4,image->columns-x),
            MagickMin(4,image->rows-y),exception);
          if (q != (Quantum *) NULL)
            SetDXT1Pixels(image,x,y,colors,bits,q);
        }

      if (SyncAuthenticPixels(image,exception) == MagickFalse)
        return MagickFalse;
    }
  }
  
  return(SkipDXTMipmaps(image,dds_info,8,exception));
}