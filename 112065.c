ModuleExport size_t RegisterPSDImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("PSD","PSB","Adobe Large Document Format");
  entry->decoder=(DecodeImageHandler *) ReadPSDImage;
  entry->encoder=(EncodeImageHandler *) WritePSDImage;
  entry->magick=(IsImageFormatHandler *) IsPSD;
  entry->flags|=CoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("PSD","PSD","Adobe Photoshop bitmap");
  entry->decoder=(DecodeImageHandler *) ReadPSDImage;
  entry->encoder=(EncodeImageHandler *) WritePSDImage;
  entry->magick=(IsImageFormatHandler *) IsPSD;
  entry->flags|=CoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}