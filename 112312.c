MagickExport MagickBooleanType IsImageGray(const Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if ((image->type == BilevelType) || (image->type == GrayscaleType) ||
      (image->type == GrayscaleAlphaType))
    return(MagickTrue);
  return(MagickFalse);
}