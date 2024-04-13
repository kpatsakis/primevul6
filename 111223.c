ModuleExport size_t RegisterPWPImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("PWP","PWP","Seattle Film Works");
  entry->decoder=(DecodeImageHandler *) ReadPWPImage;
  entry->magick=(IsImageFormatHandler *) IsPWP;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}