static MagickBooleanType ReadPSDMergedImage(const ImageInfo *image_info,
  Image *image,const PSDInfo *psd_info,ExceptionInfo *exception)
{
  MagickOffsetType
    *sizes;

  MagickBooleanType
    status;

  PSDCompressionType
    compression;

  ssize_t
    i;

  if ((image_info->number_scenes != 0) && (image_info->scene != 0))
    return(MagickTrue);
  compression=(PSDCompressionType) ReadBlobMSBShort(image);
  image->compression=ConvertPSDCompression(compression);

  if (compression != Raw && compression != RLE)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        TypeWarning,"CompressionNotSupported","'%.20g'",(double) compression);
      return(MagickFalse);
    }

  sizes=(MagickOffsetType *) NULL;
  if (compression == RLE)
    {
      sizes=ReadPSDRLESizes(image,psd_info,image->rows*psd_info->channels);
      if (sizes == (MagickOffsetType *) NULL)
        ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
          image->filename);
    }

  SetPsdMetaChannels(image,psd_info,psd_info->channels,exception);
  status=MagickTrue;
  for (i=0; i < (ssize_t) psd_info->channels; i++)
  {
    PixelChannel
      channel;

    status=GetPixelChannelFromPsdIndex(psd_info,i,&channel);
    if (status == MagickFalse)
      {
        (void) ThrowMagickException(exception,GetMagickModule(),
          CorruptImageError,"MaximumChannelsExceeded","'%.20g'",(double) i);
        break;
      }

    if (compression == RLE)
      status=ReadPSDChannelRLE(image,channel,sizes+(i*image->rows),exception);
    else
      status=ReadPSDChannelRaw(image,channel,exception);

    if (status != MagickFalse)
      status=SetImageProgress(image,LoadImagesTag,(MagickOffsetType) i,
        psd_info->channels);

    if (status == MagickFalse)
      break;
  }

  if ((status != MagickFalse) && (image->colorspace == CMYKColorspace))
    status=NegateCMYK(image,exception);

  if (status != MagickFalse)
    status=CorrectPSDAlphaBlend(image_info,image,exception);

  sizes=(MagickOffsetType *) RelinquishMagickMemory(sizes);

  return(status);
}