MagickExport ImageType IdentifyImageGray(const Image *image,
  ExceptionInfo *exception)
{
  CacheView
    *image_view;

  ImageType
    type;

  register const Quantum
    *p;

  register ssize_t
    x;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if ((image->type == BilevelType) || (image->type == GrayscaleType) ||
      (image->type == GrayscaleAlphaType))
    return(image->type);
  if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
    return(UndefinedType);
  type=BilevelType;
  image_view=AcquireVirtualCacheView(image,exception);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if (IsPixelGray(image,p) == MagickFalse)
        {
          type=UndefinedType;
          break;
        }
      if ((type == BilevelType) &&
          (IsPixelMonochrome(image,p) == MagickFalse))
        type=GrayscaleType;
      p+=GetPixelChannels(image);
    }
    if (type == UndefinedType)
      break;
  }
  image_view=DestroyCacheView(image_view);
  if ((type == GrayscaleType) && (image->alpha_trait != UndefinedPixelTrait))
    type=GrayscaleAlphaType;
  return(type);
}