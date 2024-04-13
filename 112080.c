static MagickBooleanType CorrectPSDOpacity(LayerInfo *layer_info,
  ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  ssize_t
    y;

  if (layer_info->opacity == OpaqueAlpha)
    return(MagickTrue);

  layer_info->image->alpha_trait=BlendPixelTrait;
  status=MagickTrue;
#if defined(MAGICKCORE_OPENMP_SUPPORT)
#pragma omp parallel for schedule(static,4) shared(status) \
  magick_threads(layer_info->image,layer_info->image,layer_info->image->rows,1)
#endif
  for (y=0; y < (ssize_t) layer_info->image->rows; y++)
  {
    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetAuthenticPixels(layer_info->image,0,y,layer_info->image->columns,1,
      exception);
    if (q == (Quantum *)NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) layer_info->image->columns; x++)
    {
      SetPixelAlpha(layer_info->image,(Quantum) (QuantumScale*(GetPixelAlpha(
        layer_info->image,q))*layer_info->opacity),q);
      q+=GetPixelChannels(layer_info->image);
    }
    if (SyncAuthenticPixels(layer_info->image,exception) == MagickFalse)
      status=MagickFalse;
  }

  return(status);
}