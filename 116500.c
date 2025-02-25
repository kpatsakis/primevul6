static inline void SetAssociatedAlpha(const Image *image,CubeInfo *cube_info)
{
  MagickBooleanType
    associate_alpha;

  associate_alpha=image->alpha_trait == BlendPixelTrait ? MagickTrue :
    MagickFalse;
  if ((cube_info->quantize_info->number_colors == 2) &&
      (cube_info->quantize_info->colorspace == GRAYColorspace))
    associate_alpha=MagickFalse;
  cube_info->associate_alpha=associate_alpha;
}