ModuleExport size_t RegisterLABELImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("LABEL","LABEL","Image label");
  entry->decoder=(DecodeImageHandler *) ReadLABELImage;
  entry->flags^=CoderAdjoinFlag;
  entry->format_type=ImplicitFormatType;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}