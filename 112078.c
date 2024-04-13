static inline MagickOffsetType *ReadPSDRLEOffsets(Image *image,
  const PSDInfo *psd_info,const size_t size)
{
  MagickOffsetType
    *offsets;

  ssize_t
    y;

  offsets=(MagickOffsetType *) AcquireQuantumMemory(size,sizeof(*offsets));
  if(offsets != (MagickOffsetType *) NULL)
    {
      for (y=0; y < (ssize_t) size; y++)
      {
        if (psd_info->version == 1)
          offsets[y]=(MagickOffsetType) ReadBlobShort(image);
        else
          offsets[y]=(MagickOffsetType) ReadBlobLong(image);
      }
    }
  return offsets;
}