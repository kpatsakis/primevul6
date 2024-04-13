static MagickBooleanType GetPixelChannelFromPsdIndex(const PSDInfo *psd_info,
  ssize_t index,PixelChannel *channel)
{
  *channel=RedPixelChannel;
  switch (psd_info->mode)
  {
    case BitmapMode:
    case IndexedMode:
    case GrayscaleMode:
    {
      if (index == 1)
        index=-1;
      else if (index > 1)
        index=StartMetaPixelChannel+index-2;
      break;
    }
    case LabMode:
    case MultichannelMode:
    case RGBMode:
    {
      if (index == 3)
        index=-1;
      else if (index > 3)
        index=StartMetaPixelChannel+index-4;
      break;
    }
    case CMYKMode:
    {
      if (index == 4)
        index=-1;
      else if (index > 4)
        index=StartMetaPixelChannel+index-5;
      break;
    }
  }
  if ((index < -2) || (index >= MaxPixelChannels))
    return(MagickFalse);
  if (index == -1)
    *channel=AlphaPixelChannel;
  else if (index == -2)
    *channel=ReadMaskPixelChannel;
  else
    *channel=(PixelChannel) index;
  return(MagickTrue);
}