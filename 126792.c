ModuleExport size_t RegisterCALSImage(void)
{
#define CALSDescription  "Continuous Acquisition and Life-cycle Support Type 1"
#define CALSNote  "Specified in MIL-R-28002 and MIL-PRF-28002"

  MagickInfo
    *entry;

  entry=AcquireMagickInfo("CALS","CAL",CALSDescription);
  entry->decoder=(DecodeImageHandler *) ReadCALSImage;
#if defined(MAGICKCORE_TIFF_DELEGATE)
  entry->encoder=(EncodeImageHandler *) WriteCALSImage;
#endif
  entry->flags^=CoderAdjoinFlag;
  entry->magick=(IsImageFormatHandler *) IsCALS;
  entry->note=ConstantString(CALSNote);
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("CALS","CALS",CALSDescription);
  entry->decoder=(DecodeImageHandler *) ReadCALSImage;
#if defined(MAGICKCORE_TIFF_DELEGATE)
  entry->encoder=(EncodeImageHandler *) WriteCALSImage;
#endif
  entry->flags^=CoderAdjoinFlag;
  entry->magick=(IsImageFormatHandler *) IsCALS;
  entry->note=ConstantString(CALSNote);
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}