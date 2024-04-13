static inline MagickBooleanType PSDSkipImage(const PSDInfo *psd_info,
  const ImageInfo *image_info,const size_t index)
{
  if (psd_info->has_merged_image == MagickFalse)
    return(MagickFalse);
  if (image_info->number_scenes == 0)
    return(MagickFalse);
  if (index < image_info->scene)
    return(MagickTrue);
  if (index > image_info->scene+image_info->number_scenes-1)
    return(MagickTrue);
  return(MagickFalse);
}