static MagickBooleanType ApplyPSDLayerOpacity(Image *image,Quantum opacity,
  MagickBooleanType revert,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  ssize_t
    y;

  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  applying layer opacity %.20g", (double) opacity);
  if (opacity == OpaqueAlpha)
    return(MagickTrue);
  if (image->alpha_trait != BlendPixelTrait)
    (void) SetImageAlphaChannel(image,OpaqueAlphaChannel,exception);
  status=MagickTrue;
#if defined(MAGICKCORE_OPENMP_SUPPORT)
#pragma omp parallel for schedule(static) shared(status) \
  magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    Quantum
      *magick_restrict q;

    ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if (revert == MagickFalse)
        SetPixelAlpha(image,ClampToQuantum(QuantumScale*
          GetPixelAlpha(image,q)*opacity),q);
      else if (opacity > 0)
        SetPixelAlpha(image,ClampToQuantum((double) QuantumRange*
          GetPixelAlpha(image,q)/(MagickRealType) opacity),q);
      q+=GetPixelChannels(image);
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      status=MagickFalse;
  }

  return(status);
}