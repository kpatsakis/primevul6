static void WriteOneChannel(const PSDInfo *psd_info,const ImageInfo *image_info,
  Image *image,Image *next_image,unsigned char *compact_pixels,
  const QuantumType quantum_type,const MagickBooleanType compression_flag,
  ExceptionInfo *exception)
{
  int
    y;

  MagickBooleanType
    monochrome;

  QuantumInfo
    *quantum_info;

  register const Quantum
    *p;

  register ssize_t
    i;

  size_t
    length,
    packet_size;

  unsigned char
    *pixels;

  (void) psd_info;
  if ((compression_flag != MagickFalse) &&
      (next_image->compression != RLECompression))
    (void) WriteBlobMSBShort(image,0);
  if (next_image->depth > 8)
    next_image->depth=16;
  monochrome=IsImageMonochrome(image) && (image->depth == 1) ?
    MagickTrue : MagickFalse;
  packet_size=next_image->depth > 8UL ? 2UL : 1UL;
  (void) packet_size;
  quantum_info=AcquireQuantumInfo(image_info,image);
  pixels=(unsigned char *) GetQuantumPixels(quantum_info);
  for (y=0; y < (ssize_t) next_image->rows; y++)
  {
    p=GetVirtualPixels(next_image,0,y,next_image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    length=ExportQuantumPixels(next_image,(CacheView *) NULL,quantum_info,
      quantum_type,pixels,exception);
    if (monochrome != MagickFalse)
      for (i=0; i < (ssize_t) length; i++)
        pixels[i]=(~pixels[i]);
    if (next_image->compression != RLECompression)
      (void) WriteBlob(image,length,pixels);
    else
      {
        length=PSDPackbitsEncodeImage(image,length,pixels,compact_pixels,
          exception);
        (void) WriteBlob(image,length,compact_pixels);
      }
  }
  quantum_info=DestroyQuantumInfo(quantum_info);
}