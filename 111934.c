static MagickBooleanType CheckPSDChannels(const Image *image,
  const PSDInfo *psd_info,LayerInfo *layer_info)
{
  int
    channel_type;

  size_t
    blob_size;

  ssize_t
    i;

  if (layer_info->channels < psd_info->min_channels)
    return(MagickFalse);
  channel_type=RedChannel;
  if (psd_info->min_channels >= 3)
    channel_type|=(GreenChannel | BlueChannel);
  if (psd_info->min_channels >= 4)
    channel_type|=BlackChannel;
  blob_size=(size_t) GetBlobSize(image);
  for (i=0; i < (ssize_t) layer_info->channels; i++)
  {
    PixelChannel
      channel;

    if (layer_info->channel_info[i].size >= blob_size)
      return(MagickFalse);
    if (layer_info->channel_info[i].supported == MagickFalse)
      continue;
    channel=layer_info->channel_info[i].channel;
    if ((i == 0) && (psd_info->mode == IndexedMode) &&
        (channel != RedPixelChannel))
      return(MagickFalse);
    if (channel == AlphaPixelChannel)
      {
        channel_type|=AlphaChannel;
        continue;
      }
    if (channel == RedPixelChannel)
      channel_type&=~RedChannel;
    else if (channel == GreenPixelChannel)
      channel_type&=~GreenChannel;
    else if (channel == BluePixelChannel)
      channel_type&=~BlueChannel;
    else if (channel == BlackPixelChannel)
      channel_type&=~BlackChannel;
  }
  if (channel_type == 0)
    return(MagickTrue);
  if ((channel_type == AlphaChannel) &&
      (layer_info->channels >= psd_info->min_channels + 1))
    return(MagickTrue);
  return(MagickFalse);
}