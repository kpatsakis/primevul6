MagickExport MagickBooleanType IdentifyImageMonochrome(const Image *image,
  ExceptionInfo *exception)
{
  CacheView
    *image_view;

  ImageType
    type;

  register ssize_t
    x;

  register const Quantum
    *p;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->type == BilevelType)
    return(MagickTrue);
  if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
    return(MagickFalse);
  type=BilevelType;
  image_view=AcquireVirtualCacheView(image,exception);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if (IsPixelMonochrome(image,p) == MagickFalse)
        {
          type=UndefinedType;
          break;
        }
      p+=GetPixelChannels(image);
    }
    if (type == UndefinedType)
      break;
  }
  image_view=DestroyCacheView(image_view);
  if (type == BilevelType)
    return(MagickTrue);
  return(MagickFalse);
}