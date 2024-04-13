static MagickBooleanType ReadPSDChannelRaw(Image *image,const PixelChannel channel,
  ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  size_t
    row_size;

  ssize_t
    count,
    y;

  unsigned char
    *pixels;

  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
       "      layer data is RAW");

  row_size=GetPSDRowSize(image);
  pixels=(unsigned char *) AcquireQuantumMemory(row_size,sizeof(*pixels));
  if (pixels == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);
  (void) memset(pixels,0,row_size*sizeof(*pixels));

  status=MagickTrue;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    status=MagickFalse;

    count=ReadBlob(image,row_size,pixels);
    if (count != (ssize_t) row_size)
      break;

    status=ReadPSDChannelPixels(image,y,channel,pixels,exception);
    if (status == MagickFalse)
      break;
  }

  pixels=(unsigned char *) RelinquishMagickMemory(pixels);
  return(status);
}