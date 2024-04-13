ModuleExport size_t RegisterTXTImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("TXT","SPARSE-COLOR","Sparse Color");
  entry->encoder=(EncodeImageHandler *) WriteTXTImage;
  entry->flags|=CoderRawSupportFlag;
  entry->flags|=CoderEndianSupportFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("TXT","TEXT","Text");
  entry->decoder=(DecodeImageHandler *) ReadTEXTImage;
  entry->format_type=ImplicitFormatType;
  entry->flags|=CoderRawSupportFlag;
  entry->flags|=CoderEndianSupportFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("TXT","TXT","Text");
  entry->decoder=(DecodeImageHandler *) ReadTXTImage;
  entry->encoder=(EncodeImageHandler *) WriteTXTImage;
  entry->magick=(IsImageFormatHandler *) IsTXT;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}