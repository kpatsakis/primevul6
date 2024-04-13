MagickExport MagickBooleanType CompressImageColormap(Image *image,
  ExceptionInfo *exception)
{
  QuantizeInfo
    quantize_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->storage_class != PseudoClass)
    return(MagickFalse);
  GetQuantizeInfo(&quantize_info);
  quantize_info.number_colors=image->colors;
  quantize_info.tree_depth=MaxTreeDepth;
  return(QuantizeImage(&quantize_info,image,exception));
}