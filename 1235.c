ModuleExport size_t RegisterEMFImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("EMF");
#if defined(MAGICKCORE_WINGDI32_DELEGATE)
  entry->decoder=ReadEMFImage;
#endif
  entry->description=ConstantString(
    "Windows Enhanced Meta File");
  entry->magick=(IsImageFormatHandler *) IsEMF;
  entry->blob_support=MagickFalse;
  entry->magick_module=ConstantString("EMF");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("WMF");
#if defined(MAGICKCORE_WINGDI32_DELEGATE)
  entry->decoder=ReadEMFImage;
#endif
  entry->description=ConstantString("Windows Meta File");
  entry->magick=(IsImageFormatHandler *) IsWMF;
  entry->blob_support=MagickFalse;
  entry->magick_module=ConstantString("EMF");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}