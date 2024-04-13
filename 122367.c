ModuleExport size_t RegisterMAPImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("MAP","MAP","Colormap intensities and indices");
  entry->decoder=(DecodeImageHandler *) ReadMAPImage;
  entry->encoder=(EncodeImageHandler *) WriteMAPImage;
  entry->flags^=CoderAdjoinFlag;
  entry->format_type=ExplicitFormatType;
  entry->flags|=CoderRawSupportFlag;
  entry->flags|=CoderEndianSupportFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}