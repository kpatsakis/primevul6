static void CheckMergedImageAlpha(const PSDInfo *psd_info,Image *image)
{
  /*
    The number of layers cannot be used to determine if the merged image
    contains an alpha channel. So we enable it when we think we should.
  */
  if (((psd_info->mode == GrayscaleMode) && (psd_info->channels > 1)) ||
      ((psd_info->mode == RGBMode) && (psd_info->channels > 3)) ||
      ((psd_info->mode == CMYKMode) && (psd_info->channels > 4)))
    image->alpha_trait=BlendPixelTrait;
}