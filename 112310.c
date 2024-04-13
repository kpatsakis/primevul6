MagickExport ImageType GetImageType(const Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->colorspace == CMYKColorspace)
    {
      if (image->alpha_trait == UndefinedPixelTrait)
        return(ColorSeparationType);
      return(ColorSeparationAlphaType);
    }
  if (IsImageMonochrome(image) != MagickFalse)
    return(BilevelType);
  if (IsImageGray(image) != MagickFalse)
    {
      if (image->alpha_trait != UndefinedPixelTrait)
        return(GrayscaleAlphaType);
      return(GrayscaleType);
    }
  if (IsPaletteImage(image) != MagickFalse)
    {
      if (image->alpha_trait != UndefinedPixelTrait)
        return(PaletteAlphaType);
      return(PaletteType);
    }
  if (image->alpha_trait != UndefinedPixelTrait)
    return(TrueColorAlphaType);
  return(TrueColorType);
}