static void SetPsdMetaChannels(Image *image,const PSDInfo *psd_info,
  const unsigned short channels,ExceptionInfo *exception)
{
  ssize_t
    number_meta_channels;

  number_meta_channels=(ssize_t) channels-psd_info->min_channels;
  if (image->alpha_trait == BlendPixelTrait)
    number_meta_channels--;
  if (number_meta_channels > 0)
    (void) SetPixelMetaChannels(image,(size_t) number_meta_channels,exception);
}