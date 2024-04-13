ModuleExport size_t RegisterINLINEImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("DATA","INLINE","Base64-encoded inline images");
  entry->decoder=(DecodeImageHandler *) ReadINLINEImage;
  entry->encoder=(EncodeImageHandler *) WriteINLINEImage;
  entry->format_type=ImplicitFormatType;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("INLINE","INLINE","Base64-encoded inline images");
  entry->decoder=(DecodeImageHandler *) ReadINLINEImage;
  entry->encoder=(EncodeImageHandler *) WriteINLINEImage;
  entry->format_type=ImplicitFormatType;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}