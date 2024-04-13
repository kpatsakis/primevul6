ModuleExport size_t RegisterDDSImage(void)
{
  MagickInfo
    *entry;

  entry = AcquireMagickInfo("DDS","DDS","Microsoft DirectDraw Surface");
  entry->decoder = (DecodeImageHandler *) ReadDDSImage;
  entry->encoder = (EncodeImageHandler *) WriteDDSImage;
  entry->magick = (IsImageFormatHandler *) IsDDS;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  entry = AcquireMagickInfo("DDS","DXT1","Microsoft DirectDraw Surface");
  entry->decoder = (DecodeImageHandler *) ReadDDSImage;
  entry->encoder = (EncodeImageHandler *) WriteDDSImage;
  entry->magick = (IsImageFormatHandler *) IsDDS;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  entry = AcquireMagickInfo("DDS","DXT5","Microsoft DirectDraw Surface");
  entry->decoder = (DecodeImageHandler *) ReadDDSImage;
  entry->encoder = (EncodeImageHandler *) WriteDDSImage;
  entry->magick = (IsImageFormatHandler *) IsDDS;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}