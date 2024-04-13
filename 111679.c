ModuleExport size_t RegisterARTImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("ART","ART","PFS: 1st Publisher Clip Art");
  entry->decoder=(DecodeImageHandler *) ReadARTImage;
  entry->encoder=(EncodeImageHandler *) WriteARTImage;
  entry->flags|=CoderRawSupportFlag;
  entry->flags^=CoderAdjoinFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}