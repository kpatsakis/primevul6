static unsigned short ReadDCMShort(DCMStreamInfo *stream_info,Image *image)
{
  int
    shift;

  unsigned short
    value;

  if (image->compression != RLECompression)
    return(ReadBlobLSBShort(image));
  shift=image->depth < 16 ? 4 : 8;
  value=ReadDCMByte(stream_info,image) | (unsigned short)
    (ReadDCMByte(stream_info,image) << shift);
  return(value);
}