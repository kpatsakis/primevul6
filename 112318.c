MagickExport MagickBooleanType IsImageMonochrome(const Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->type == BilevelType)
    return(MagickTrue);
  return(MagickFalse);
}