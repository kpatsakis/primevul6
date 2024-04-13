MagickExport MagickBooleanType IsImageOpaque(const Image *image,
  ExceptionInfo *exception)
{
  CacheView
    *image_view;

  register const Quantum
    *p;

  register ssize_t
    x;

  ssize_t
    y;

  /*
    Determine if image is opaque.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->alpha_trait == UndefinedPixelTrait)
    return(MagickTrue);
  image_view=AcquireVirtualCacheView(image,exception);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if (GetPixelAlpha(image,p) != OpaqueAlpha)
        break;
      p+=GetPixelChannels(image);
    }
    if (x < (ssize_t) image->columns)
      break;
  }
  image_view=DestroyCacheView(image_view);
  return(y < (ssize_t) image->rows ? MagickFalse : MagickTrue);
}