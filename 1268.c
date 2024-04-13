static size_t WriteCompressionStart(const PSDInfo *psd_info,Image *image,
  const Image *next_image,const ssize_t channels)
{
  ssize_t
    i,
    offset,
    y;

  if (next_image->compression == RLECompression)
    {
      offset=WriteBlobMSBShort(image,RLE);
      for (i=0; i < channels; i++)
        for (y=0; y < (ssize_t) next_image->rows; y++)
          offset+=SetPSDOffset(psd_info,image,0);
    }
#ifdef MAGICKCORE_ZLIB_DELEGATE
  else if (next_image->compression == ZipCompression)
    offset=WriteBlobMSBShort(image,ZipWithoutPrediction);
#endif
  else
    offset=WriteBlobMSBShort(image,Raw);
  return((size_t) offset);
}