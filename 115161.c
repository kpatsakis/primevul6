static Image *ReadXBMImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    buffer[MagickPathExtent],
    name[MagickPathExtent];

  Image
    *image;

  int
    c;

  MagickBooleanType
    status;

  register ssize_t
    i,
    x;

  register Quantum
    *q;

  register unsigned char
    *p;

  short int
    hex_digits[256];

  ssize_t
    y;

  unsigned char
    *data;

  unsigned int
    bit,
    byte,
    bytes_per_line,
    height,
    length,
    padding,
    version,
    width;

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
    Read X bitmap header.
  */
  width=0;
  height=0;
  while (ReadBlobString(image,buffer) != (char *) NULL)
    if (sscanf(buffer,"#define %32s %u",name,&width) == 2)
      if ((strlen(name) >= 6) &&
          (LocaleCompare(name+strlen(name)-6,"_width") == 0))
        break;
  while (ReadBlobString(image,buffer) != (char *) NULL)
    if (sscanf(buffer,"#define %32s %u",name,&height) == 2)
      if ((strlen(name) >= 7) &&
          (LocaleCompare(name+strlen(name)-7,"_height") == 0))
        break;
  image->columns=width;
  image->rows=height;
  image->depth=8;
  image->storage_class=PseudoClass;
  image->colors=2;
  /*
    Scan until hex digits.
  */
  version=11;
  while (ReadBlobString(image,buffer) != (char *) NULL)
  {
    if (sscanf(buffer,"static short %32s = {",name) == 1)
      version=10;
    else
      if (sscanf(buffer,"static unsigned char %32s = {",name) == 1)
        version=11;
      else
        if (sscanf(buffer,"static char %32s = {",name) == 1)
          version=11;
        else
          continue;
    p=(unsigned char *) strrchr(name,'_');
    if (p == (unsigned char *) NULL)
      p=(unsigned char *) name;
    else
      p++;
    if (LocaleCompare("bits[]",(char *) p) == 0)
      break;
  }
  if ((image->columns == 0) || (image->rows == 0) ||
      (EOFBlob(image) != MagickFalse))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  /*
    Initialize image structure.
  */
  if (AcquireImageColormap(image,image->colors,exception) == MagickFalse)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  /*
    Initialize colormap.
  */
  image->colormap[0].red=(MagickRealType) QuantumRange;
  image->colormap[0].green=(MagickRealType) QuantumRange;
  image->colormap[0].blue=(MagickRealType) QuantumRange;
  image->colormap[1].red=0.0;
  image->colormap[1].green=0.0;
  image->colormap[1].blue=0.0;
  if (image_info->ping != MagickFalse)
    {
      (void) CloseBlob(image);
      return(GetFirstImageInList(image));
    }
  status=SetImageExtent(image,image->columns,image->rows,exception);
  if (status == MagickFalse)
    return(DestroyImageList(image));
  /*
    Initialize hex values.
  */
  hex_digits[(int) '0']=0;
  hex_digits[(int) '1']=1;
  hex_digits[(int) '2']=2;
  hex_digits[(int) '3']=3;
  hex_digits[(int) '4']=4;
  hex_digits[(int) '5']=5;
  hex_digits[(int) '6']=6;
  hex_digits[(int) '7']=7;
  hex_digits[(int) '8']=8;
  hex_digits[(int) '9']=9;
  hex_digits[(int) 'A']=10;
  hex_digits[(int) 'B']=11;
  hex_digits[(int) 'C']=12;
  hex_digits[(int) 'D']=13;
  hex_digits[(int) 'E']=14;
  hex_digits[(int) 'F']=15;
  hex_digits[(int) 'a']=10;
  hex_digits[(int) 'b']=11;
  hex_digits[(int) 'c']=12;
  hex_digits[(int) 'd']=13;
  hex_digits[(int) 'e']=14;
  hex_digits[(int) 'f']=15;
  hex_digits[(int) 'x']=0;
  hex_digits[(int) ' ']=(-1);
  hex_digits[(int) ',']=(-1);
  hex_digits[(int) '}']=(-1);
  hex_digits[(int) '\n']=(-1);
  hex_digits[(int) '\t']=(-1);
  /*
    Read hex image data.
  */
  padding=0;
  if (((image->columns % 16) != 0) && ((image->columns % 16) < 9) &&
      (version == 10))
    padding=1;
  bytes_per_line=(unsigned int) (image->columns+7)/8+padding;
  length=(unsigned int) image->rows;
  data=(unsigned char *) AcquireQuantumMemory(length,bytes_per_line*
    sizeof(*data));
  if (data == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  p=data;
  if (version == 10)
    for (i=0; i < (ssize_t) (bytes_per_line*image->rows); (i+=2))
    {
      c=XBMInteger(image,hex_digits);
      if (c < 0)
        break;
      *p++=(unsigned char) c;
      if ((padding == 0) || (((i+2) % bytes_per_line) != 0))
        *p++=(unsigned char) (c >> 8);
    }
  else
    for (i=0; i < (ssize_t) (bytes_per_line*image->rows); i++)
    {
      c=XBMInteger(image,hex_digits);
      if (c < 0)
        break;
      *p++=(unsigned char) c;
    }
  if (EOFBlob(image) != MagickFalse)
    {
      data=(unsigned char *) RelinquishMagickMemory(data);
      ThrowReaderException(CorruptImageError,"UnexpectedEndOfFile");
    }
  /*
    Convert X bitmap image to pixel packets.
  */
  p=data;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (Quantum *) NULL)
      break;
    bit=0;
    byte=0;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if (bit == 0)
        byte=(unsigned int) (*p++);
      SetPixelIndex(image,(Quantum) ((byte & 0x01) != 0 ? 0x01 : 0x00),q);
      bit++;
      byte>>=1;
      if (bit == 8)
        bit=0;
      q+=GetPixelChannels(image);
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  data=(unsigned char *) RelinquishMagickMemory(data);
  (void) SyncImage(image,exception);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}