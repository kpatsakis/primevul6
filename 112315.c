MagickExport RectangleInfo GetImageBoundingBox(const Image *image,
  ExceptionInfo *exception)
{
  CacheView
    *image_view;

  MagickBooleanType
    status;

  PixelInfo
    target[3],
    zero;

  RectangleInfo
    bounds;

  register const Quantum
    *r;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  bounds.width=0;
  bounds.height=0;
  bounds.x=(ssize_t) image->columns;
  bounds.y=(ssize_t) image->rows;
  GetPixelInfo(image,&target[0]);
  image_view=AcquireVirtualCacheView(image,exception);
  r=GetCacheViewVirtualPixels(image_view,0,0,1,1,exception);
  if (r == (const Quantum *) NULL)
    {
      image_view=DestroyCacheView(image_view);
      return(bounds);
    }
  GetPixelInfoPixel(image,r,&target[0]);
  GetPixelInfo(image,&target[1]);
  r=GetCacheViewVirtualPixels(image_view,(ssize_t) image->columns-1,0,1,1,
    exception);
  if (r != (const Quantum *) NULL)
    GetPixelInfoPixel(image,r,&target[1]);
  GetPixelInfo(image,&target[2]);
  r=GetCacheViewVirtualPixels(image_view,0,(ssize_t) image->rows-1,1,1,
    exception);
  if (r != (const Quantum *) NULL)
    GetPixelInfoPixel(image,r,&target[2]);
  status=MagickTrue;
  GetPixelInfo(image,&zero);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(status) \
    magick_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    PixelInfo
      pixel;

    RectangleInfo
      bounding_box;

    register const Quantum
      *magick_restrict p;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
#if defined(MAGICKCORE_OPENMP_SUPPORT)
#  pragma omp critical (MagickCore_GetImageBoundingBox)
#endif
    bounding_box=bounds;
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    pixel=zero;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      GetPixelInfoPixel(image,p,&pixel);
      if ((x < bounding_box.x) &&
          (IsFuzzyEquivalencePixelInfo(&pixel,&target[0]) == MagickFalse))
        bounding_box.x=x;
      if ((x > (ssize_t) bounding_box.width) &&
          (IsFuzzyEquivalencePixelInfo(&pixel,&target[1]) == MagickFalse))
        bounding_box.width=(size_t) x;
      if ((y < bounding_box.y) &&
          (IsFuzzyEquivalencePixelInfo(&pixel,&target[0]) == MagickFalse))
        bounding_box.y=y;
      if ((y > (ssize_t) bounding_box.height) &&
          (IsFuzzyEquivalencePixelInfo(&pixel,&target[2]) == MagickFalse))
        bounding_box.height=(size_t) y;
      p+=GetPixelChannels(image);
    }
#if defined(MAGICKCORE_OPENMP_SUPPORT)
#  pragma omp critical (MagickCore_GetImageBoundingBox)
#endif
    {
      if (bounding_box.x < bounds.x)
        bounds.x=bounding_box.x;
      if (bounding_box.y < bounds.y)
        bounds.y=bounding_box.y;
      if (bounding_box.width > bounds.width)
        bounds.width=bounding_box.width;
      if (bounding_box.height > bounds.height)
        bounds.height=bounding_box.height;
    }
  }
  image_view=DestroyCacheView(image_view);
  if ((bounds.width == 0) && (bounds.height == 0))
    (void) ThrowMagickException(exception,GetMagickModule(),OptionWarning,
      "GeometryDoesNotContainImage","`%s'",image->filename);
  else
    {
      bounds.width-=(bounds.x-1);
      bounds.height-=(bounds.y-1);
    }
  return(bounds);
}