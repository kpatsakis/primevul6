static inline ssize_t WritePSDSize(const PSDInfo *psd_info,Image *image,
  const MagickSizeType size,const MagickOffsetType offset)
{
  MagickOffsetType
    current_offset;

  ssize_t
    result;

  current_offset=TellBlob(image);
  (void) SeekBlob(image,offset,SEEK_SET);
  if (psd_info->version == 1)
    result=WriteBlobMSBLong(image,(unsigned int) size);
  else
    result=WriteBlobMSBLongLong(image,size);
  (void) SeekBlob(image,current_offset,SEEK_SET);
  return(result);
}