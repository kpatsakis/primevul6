ModuleExport size_t RegisterXBMImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("XBM","XBM",
    "X Windows system bitmap (black and white)");
  entry->decoder=(DecodeImageHandler *) ReadXBMImage;
  entry->encoder=(EncodeImageHandler *) WriteXBMImage;
  entry->magick=(IsImageFormatHandler *) IsXBM;
  entry->flags^=CoderAdjoinFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}