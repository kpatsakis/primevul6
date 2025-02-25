static Image *ReadSIXELImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    *sixel_buffer;

  Image
    *image;

  MagickBooleanType
    status;

  register char
    *p;

  register ssize_t
    x;

  register Quantum
    *q;

  size_t
    length;

  ssize_t
    i,
    j,
    y;

  unsigned char
    *sixel_pixels,
    *sixel_palette;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image=AcquireImage(image_info,exception);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Read SIXEL file.
  */
  length=MagickPathExtent;
  sixel_buffer=(char *) AcquireQuantumMemory((size_t) length,
    sizeof(*sixel_buffer));
  p=sixel_buffer;
  if (sixel_buffer != (char *) NULL)
    while (ReadBlobString(image,p) != (char *) NULL)
    {
      if ((*p == '#') && ((p == sixel_buffer) || (*(p-1) == '\n')))
        continue;
      if ((*p == '}') && (*(p+1) == ';'))
        break;
      p+=strlen(p);
      if ((size_t) (p-sixel_buffer+MagickPathExtent) < length)
        continue;
      length<<=1;
      sixel_buffer=(char *) ResizeQuantumMemory(sixel_buffer,length+
        MagickPathExtent,sizeof(*sixel_buffer));
      if (sixel_buffer == (char *) NULL)
        break;
      p=sixel_buffer+strlen(sixel_buffer);
    }
  if (sixel_buffer == (char *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  /*
    Decode SIXEL
  */
  if (sixel_decode((unsigned char *) sixel_buffer,&sixel_pixels,&image->columns,&image->rows,&sixel_palette,&image->colors) == MagickFalse)
    {
      sixel_buffer=(char *) RelinquishMagickMemory(sixel_buffer);
      ThrowReaderException(CorruptImageError,"CorruptImage");
    }
  sixel_buffer=(char *) RelinquishMagickMemory(sixel_buffer);
  image->depth=24;
  image->storage_class=PseudoClass;
  status=SetImageExtent(image,image->columns,image->rows,exception);
  if (status == MagickFalse)
    return(DestroyImageList(image));

  if (AcquireImageColormap(image,image->colors, exception) == MagickFalse)
    {
      sixel_pixels=(unsigned char *) RelinquishMagickMemory(sixel_pixels);
      sixel_palette=(unsigned char *) RelinquishMagickMemory(sixel_palette);
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    }
  for (i = 0; i < (ssize_t) image->colors; ++i) {
    image->colormap[i].red   = ScaleCharToQuantum(sixel_palette[i * 4 + 0]);
    image->colormap[i].green = ScaleCharToQuantum(sixel_palette[i * 4 + 1]);
    image->colormap[i].blue  = ScaleCharToQuantum(sixel_palette[i * 4 + 2]);
  }

  j=0;
  if (image_info->ping == MagickFalse)
    {
      /*
        Read image pixels.
      */
      for (y=0; y < (ssize_t) image->rows; y++)
      {
        q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
        if (q == (Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) image->columns; x++)
        {
          j=(ssize_t) sixel_pixels[y * image->columns + x];
          SetPixelIndex(image,j,q);
          q+=GetPixelChannels(image);
        }
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
      }
      if (y < (ssize_t) image->rows)
        {
          sixel_pixels=(unsigned char *) RelinquishMagickMemory(sixel_pixels);
          sixel_palette=(unsigned char *) RelinquishMagickMemory(sixel_palette);
          ThrowReaderException(CorruptImageError,"NotEnoughPixelData");
        }
    }
  /*
    Relinquish resources.
  */
  sixel_pixels=(unsigned char *) RelinquishMagickMemory(sixel_pixels);
  sixel_palette=(unsigned char *) RelinquishMagickMemory(sixel_palette);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}