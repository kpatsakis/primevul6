MagickExport MagickBooleanType SetImageDepth(Image *image,
  const size_t depth,ExceptionInfo *exception)
{
  CacheView
    *image_view;

  MagickBooleanType
    status;

  QuantumAny
    range;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->signature == MagickCoreSignature);
  if (depth >= MAGICKCORE_QUANTUM_DEPTH)
    {
      image->depth=depth;
      return(MagickTrue);
    }
  range=GetQuantumRange(depth);
  if (image->storage_class == PseudoClass)
    {
      register ssize_t
        i;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
      #pragma omp parallel for schedule(static,4) shared(status) \
        magick_threads(image,image,1,1)
#endif
      for (i=0; i < (ssize_t) image->colors; i++)
      {
        if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
          image->colormap[i].red=(double) ScaleAnyToQuantum(ScaleQuantumToAny(
            ClampPixel(image->colormap[i].red),range),range);
        if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
          image->colormap[i].green=(double) ScaleAnyToQuantum(ScaleQuantumToAny(
            ClampPixel(image->colormap[i].green),range),range);
        if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
          image->colormap[i].blue=(double) ScaleAnyToQuantum(ScaleQuantumToAny(
            ClampPixel(image->colormap[i].blue),range),range);
        if ((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0)
          image->colormap[i].alpha=(double) ScaleAnyToQuantum(ScaleQuantumToAny(
            ClampPixel(image->colormap[i].alpha),range),range);
      }
    }
  status=MagickTrue;
  image_view=AcquireAuthenticCacheView(image,exception);
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  if (QuantumRange <= MaxMap)
    {
      Quantum
        *depth_map;

      register ssize_t
        i;

      /*
        Scale pixels to desired (optimized with depth map).
      */
      depth_map=(Quantum *) AcquireQuantumMemory(MaxMap+1,sizeof(*depth_map));
      if (depth_map == (Quantum *) NULL)
        ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
      for (i=0; i <= (ssize_t) MaxMap; i++)
        depth_map[i]=ScaleAnyToQuantum(ScaleQuantumToAny((Quantum) i,range),
          range);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
      #pragma omp parallel for schedule(static,4) shared(status) \
        magick_threads(image,image,image->rows,1)
#endif
      for (y=0; y < (ssize_t) image->rows; y++)
      {
        register ssize_t
          x;

        register Quantum
          *magick_restrict q;

        if (status == MagickFalse)
          continue;
        q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,
          exception);
        if (q == (Quantum *) NULL)
          {
            status=MagickFalse;
            continue;
          }
        for (x=0; x < (ssize_t) image->columns; x++)
        {
          register ssize_t
            i;

          if (GetPixelReadMask(image,q) == 0)
            {
              q+=GetPixelChannels(image);
              continue;
            }
          for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
          {
            PixelChannel
              channel;

            PixelTrait
              traits;

            channel=GetPixelChannelChannel(image,i);
            traits=GetPixelChannelTraits(image,channel);
            if ((traits == UndefinedPixelTrait) ||
                (channel == IndexPixelChannel) ||
                (channel == ReadMaskPixelChannel))
              continue;
            q[i]=depth_map[ScaleQuantumToMap(q[i])];
          }
          q+=GetPixelChannels(image);
        }
        if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
          {
            status=MagickFalse;
            continue;
          }
      }
      image_view=DestroyCacheView(image_view);
      depth_map=(Quantum *) RelinquishMagickMemory(depth_map);
      if (status != MagickFalse)
        image->depth=depth;
      return(status);
    }
#endif
  /*
    Scale pixels to desired depth.
  */
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(status) \
    magick_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register ssize_t
      x;

    register Quantum
      *magick_restrict q;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      register ssize_t
        i;

      if (GetPixelReadMask(image,q) == 0)
        {
          q+=GetPixelChannels(image);
          continue;
        }
      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        PixelChannel
          channel;

        PixelTrait
          traits;

        channel=GetPixelChannelChannel(image,i);
        traits=GetPixelChannelTraits(image,channel);
        if ((traits == UndefinedPixelTrait) || (channel == IndexPixelChannel) ||
            (channel == ReadMaskPixelChannel))
          continue;
        q[i]=ScaleAnyToQuantum(ScaleQuantumToAny(ClampPixel(q[i]),range),range);
      }
      q+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      {
        status=MagickFalse;
        continue;
      }
  }
  image_view=DestroyCacheView(image_view);
  if (status != MagickFalse)
    image->depth=depth;
  return(status);
}