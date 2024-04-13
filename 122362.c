ModuleExport size_t RegisterSIXELImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("SIXEL","SIXEL","DEC SIXEL Graphics Format");
  entry->decoder=(DecodeImageHandler *) ReadSIXELImage;
  entry->encoder=(EncodeImageHandler *) WriteSIXELImage;
  entry->magick=(IsImageFormatHandler *) IsSIXEL;
  entry->flags^=CoderAdjoinFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("SIX","SIX","DEC SIXEL Graphics Format");
  entry->decoder=(DecodeImageHandler *) ReadSIXELImage;
  entry->encoder=(EncodeImageHandler *) WriteSIXELImage;
  entry->magick=(IsImageFormatHandler *) IsSIXEL;
  entry->flags^=CoderAdjoinFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}