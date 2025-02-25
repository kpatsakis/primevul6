MagickExport MagickBooleanType RemapImage(const QuantizeInfo *quantize_info,
  Image *image,const Image *remap_image,ExceptionInfo *exception)
{
  CubeInfo
    *cube_info;

  MagickBooleanType
    status;

  /*
    Initialize color cube.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(remap_image != (Image *) NULL);
  assert(remap_image->signature == MagickCoreSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  cube_info=GetCubeInfo(quantize_info,MaxTreeDepth,
    quantize_info->number_colors);
  if (cube_info == (CubeInfo *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);
  status=ClassifyImageColors(cube_info,remap_image,exception);
  if (status != MagickFalse)
    {
      /*
        Classify image colors from the reference image.
      */
      cube_info->quantize_info->number_colors=cube_info->colors;
      status=AssignImageColors(image,cube_info,exception);
    }
  DestroyCubeInfo(cube_info);
  return(status);
}