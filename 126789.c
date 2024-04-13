ModuleExport size_t RegisterSCREENSHOTImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("SCREENSHOT","SCREENSHOT","Screen shot");
  entry->decoder=(DecodeImageHandler *) ReadSCREENSHOTImage;
  entry->format_type=ImplicitFormatType;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}