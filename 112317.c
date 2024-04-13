MagickExport ImageType IdentifyImageType(const Image *image,
  ExceptionInfo *exception)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->colorspace == CMYKColorspace)
    {
      if (image->alpha_trait == UndefinedPixelTrait)
        return(ColorSeparationType);
      return(ColorSeparationAlphaType);
    }
  if (IdentifyImageMonochrome(image,exception) != MagickFalse)
    return(BilevelType);
  if (IdentifyImageGray(image,exception) != UndefinedType)
    {
      if (image->alpha_trait != UndefinedPixelTrait)
        return(GrayscaleAlphaType);
      return(GrayscaleType);
    }
  if (IdentifyPaletteImage(image,exception) != MagickFalse)
    {
      if (image->alpha_trait != UndefinedPixelTrait)
        return(PaletteAlphaType);
      return(PaletteType);
    }
  if (image->alpha_trait != UndefinedPixelTrait)
    return(TrueColorAlphaType);
  return(TrueColorType);
}