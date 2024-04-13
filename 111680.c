static MagickBooleanType WriteARTImage(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  QuantumInfo
    *quantum_info;

  register const Quantum
    *p;

  size_t
    length;

  ssize_t
    count,
    y;

  unsigned char
    *pixels;

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
  if ((image->columns > 65535UL) || (image->rows > 65535UL))
    ThrowWriterException(ImageError,"WidthOrHeightExceedsLimit");
  (void) TransformImageColorspace(image,sRGBColorspace,exception);
  (void) SetImageType(image,BilevelType,exception);
  image->endian=MSBEndian;
  image->depth=1;
  (void) WriteBlobLSBShort(image,0);
  (void) WriteBlobLSBShort(image,(unsigned short) image->columns);
  (void) WriteBlobLSBShort(image,0);
  (void) WriteBlobLSBShort(image,(unsigned short) image->rows);
  quantum_info=AcquireQuantumInfo(image_info,image);
  pixels=(unsigned char *) GetQuantumPixels(quantum_info);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    length=ExportQuantumPixels(image,(CacheView *) NULL,quantum_info,
      GrayQuantum,pixels,exception);
    count=WriteBlob(image,length,pixels);
    if (count != (ssize_t) length)
      ThrowWriterException(CorruptImageError,"UnableToWriteImageData");
    count=WriteBlob(image,(size_t) (-(ssize_t) length) & 0x01,pixels);
    status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  quantum_info=DestroyQuantumInfo(quantum_info);
  (void) CloseBlob(image);
  return(status);
}