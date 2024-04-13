static MagickBooleanType WriteXBMImage(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
  char
    basename[MagickPathExtent],
    buffer[MagickPathExtent];

  MagickBooleanType
    status;

  register const Quantum
    *p;

  register ssize_t
    x;

  size_t
    bit,
    byte;

  ssize_t
    count,
    y;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  (void) TransformImageColorspace(image,sRGBColorspace,exception);
  /*
    Write X bitmap header.
  */
  GetPathComponent(image->filename,BasePath,basename);
  (void) FormatLocaleString(buffer,MagickPathExtent,"#define %s_width %.20g\n",
    basename,(double) image->columns);
  (void) WriteBlob(image,strlen(buffer),(unsigned char *) buffer);
  (void) FormatLocaleString(buffer,MagickPathExtent,"#define %s_height %.20g\n",
    basename,(double) image->rows);
  (void) WriteBlob(image,strlen(buffer),(unsigned char *) buffer);
  (void) FormatLocaleString(buffer,MagickPathExtent,
    "static char %s_bits[] = {\n",basename);
  (void) WriteBlob(image,strlen(buffer),(unsigned char *) buffer);
  (void) CopyMagickString(buffer," ",MagickPathExtent);
  (void) WriteBlob(image,strlen(buffer),(unsigned char *) buffer);
  /*
    Convert MIFF to X bitmap pixels.
  */
  (void) SetImageType(image,BilevelType,exception);
  bit=0;
  byte=0;
  count=0;
  x=0;
  y=0;
  (void) CopyMagickString(buffer," ",MagickPathExtent);
  (void) WriteBlob(image,strlen(buffer),(unsigned char *) buffer);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      byte>>=1;
      if (GetPixelLuma(image,p) < (QuantumRange/2))
        byte|=0x80;
      bit++;
      if (bit == 8)
        {
          /*
            Write a bitmap byte to the image file.
          */
          (void) FormatLocaleString(buffer,MagickPathExtent,"0x%02X, ",
            (unsigned int) (byte & 0xff));
          (void) WriteBlob(image,strlen(buffer),(unsigned char *) buffer);
          count++;
          if (count == 12)
            {
              (void) CopyMagickString(buffer,"\n  ",MagickPathExtent);
              (void) WriteBlob(image,strlen(buffer),(unsigned char *) buffer);
              count=0;
            };
          bit=0;
          byte=0;
        }
        p+=GetPixelChannels(image);
      }
    if (bit != 0)
      {
        /*
          Write a bitmap byte to the image file.
        */
        byte>>=(8-bit);
        (void) FormatLocaleString(buffer,MagickPathExtent,"0x%02X, ",
          (unsigned int) (byte & 0xff));
        (void) WriteBlob(image,strlen(buffer),(unsigned char *) buffer);
        count++;
        if (count == 12)
          {
            (void) CopyMagickString(buffer,"\n  ",MagickPathExtent);
            (void) WriteBlob(image,strlen(buffer),(unsigned char *) buffer);
            count=0;
          };
        bit=0;
        byte=0;
      };
    status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  (void) CopyMagickString(buffer,"};\n",MagickPathExtent);
  (void) WriteBlob(image,strlen(buffer),(unsigned char *) buffer);
  (void) CloseBlob(image);
  return(MagickTrue);
}