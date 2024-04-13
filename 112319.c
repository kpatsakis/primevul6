MagickExport size_t GetImageDepth(const Image *image,ExceptionInfo *exception)
{
  CacheView
    *image_view;

  MagickBooleanType
    status;

  register ssize_t
    i;

  size_t
    *current_depth,
    depth,
    number_threads;

  ssize_t
    y;

  /*
    Compute image depth.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  number_threads=(size_t) GetMagickResourceLimit(ThreadResource);
  current_depth=(size_t *) AcquireQuantumMemory(number_threads,
    sizeof(*current_depth));
  if (current_depth == (size_t *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  status=MagickTrue;
  for (i=0; i < (ssize_t) number_threads; i++)
    current_depth[i]=1;
  if ((image->storage_class == PseudoClass) &&
      (image->alpha_trait == UndefinedPixelTrait))
    {
#if defined(MAGICKCORE_OPENMP_SUPPORT)
      #pragma omp parallel for schedule(static,4) shared(status) \
        if ((image->colors) > 256) \
          num_threads(GetMagickResourceLimit(ThreadResource))
#endif
      for (i=0; i < (ssize_t) image->colors; i++)
      {
        const int
          id = GetOpenMPThreadId();

        while (current_depth[id] < MAGICKCORE_QUANTUM_DEPTH)
        {
          MagickBooleanType
            atDepth;

          QuantumAny
            range;

          atDepth=MagickTrue;
          range=GetQuantumRange(current_depth[id]);
          if ((atDepth != MagickFalse) &&
              (GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
            if (IsPixelAtDepth(image->colormap[i].red,range) == MagickFalse)
              atDepth=MagickFalse;
          if ((atDepth != MagickFalse) &&
              (GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
            if (IsPixelAtDepth(image->colormap[i].green,range) == MagickFalse)
              atDepth=MagickFalse;
          if ((atDepth != MagickFalse) &&
              (GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
            if (IsPixelAtDepth(image->colormap[i].blue,range) == MagickFalse)
              atDepth=MagickFalse;
          if ((atDepth != MagickFalse))
            break;
          current_depth[id]++;
        }
      }
      depth=current_depth[0];
      for (i=1; i < (ssize_t) number_threads; i++)
        if (depth < current_depth[i])
          depth=current_depth[i];
      current_depth=(size_t *) RelinquishMagickMemory(current_depth);
      return(depth);
    }
  image_view=AcquireVirtualCacheView(image,exception);
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  if (QuantumRange <= MaxMap)
    {
      size_t
        *depth_map;

      /*
        Scale pixels to desired (optimized with depth map).
      */
      depth_map=(size_t *) AcquireQuantumMemory(MaxMap+1,sizeof(*depth_map));
      if (depth_map == (size_t *) NULL)
        ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
      for (i=0; i <= (ssize_t) MaxMap; i++)
      {
        unsigned int
          depth;

        for (depth=1; depth < MAGICKCORE_QUANTUM_DEPTH; depth++)
        {
          Quantum
            pixel;

          QuantumAny
            range;

          range=GetQuantumRange(depth);
          pixel=(Quantum) i;
          if (pixel == ScaleAnyToQuantum(ScaleQuantumToAny(pixel,range),range))
            break;
        }
        depth_map[i]=depth;
      }
#if defined(MAGICKCORE_OPENMP_SUPPORT)
      #pragma omp parallel for schedule(static,4) shared(status) \
        magick_threads(image,image,image->rows,1)
#endif
      for (y=0; y < (ssize_t) image->rows; y++)
      {
        const int
          id = GetOpenMPThreadId();

        register const Quantum
          *magick_restrict p;

        register ssize_t
          x;

        if (status == MagickFalse)
          continue;
        p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
        if (p == (const Quantum *) NULL)
          continue;
        for (x=0; x < (ssize_t) image->columns; x++)
        {
          if (GetPixelReadMask(image,p) == 0)
            {
              p+=GetPixelChannels(image);
              continue;
            }
          for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
          {
            PixelChannel channel=GetPixelChannelChannel(image,i);
            PixelTrait traits=GetPixelChannelTraits(image,channel);
            if ((traits == UndefinedPixelTrait) ||
                (channel == IndexPixelChannel) ||
                (channel == ReadMaskPixelChannel) ||
                (channel == MetaPixelChannel))
              continue;
            if (depth_map[ScaleQuantumToMap(p[i])] > current_depth[id])
              current_depth[id]=depth_map[ScaleQuantumToMap(p[i])];
          }
          p+=GetPixelChannels(image);
        }
        if (current_depth[id] == MAGICKCORE_QUANTUM_DEPTH)
          status=MagickFalse;
      }
      image_view=DestroyCacheView(image_view);
      depth=current_depth[0];
      for (i=1; i < (ssize_t) number_threads; i++)
        if (depth < current_depth[i])
          depth=current_depth[i];
      depth_map=(size_t *) RelinquishMagickMemory(depth_map);
      current_depth=(size_t *) RelinquishMagickMemory(current_depth);
      return(depth);
    }
#endif
  /*
    Compute pixel depth.
  */
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(status) \
    magick_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    const int
      id = GetOpenMPThreadId();

    register const Quantum
      *magick_restrict p;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      continue;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if (GetPixelReadMask(image,p) == 0)
        {
          p+=GetPixelChannels(image);
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
        while (current_depth[id] < MAGICKCORE_QUANTUM_DEPTH)
        {
          QuantumAny
            range;

          range=GetQuantumRange(current_depth[id]);
          if (p[i] == ScaleAnyToQuantum(ScaleQuantumToAny(p[i],range),range))
            break;
          current_depth[id]++;
        }
      }
      p+=GetPixelChannels(image);
    }
    if (current_depth[id] == MAGICKCORE_QUANTUM_DEPTH)
      status=MagickFalse;
  }
  image_view=DestroyCacheView(image_view);
  depth=current_depth[0];
  for (i=1; i < (ssize_t) number_threads; i++)
    if (depth < current_depth[i])
      depth=current_depth[i];
  current_depth=(size_t *) RelinquishMagickMemory(current_depth);
  return(depth);
}