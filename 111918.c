static MagickSizeType GetLayerInfoSize(const PSDInfo *psd_info,Image *image)
{
  char
    type[4];

  MagickSizeType
    size;

  ssize_t
    count;

  size=GetPSDSize(psd_info,image);
  if (size != 0)
    return(size);
  (void) ReadBlobLong(image);
  count=ReadPSDString(image,type,4);
  if ((count != 4) || (LocaleNCompare(type,"8BIM",4) != 0))
    return(0);
  count=ReadPSDString(image,type,4);
  if ((count == 4) && ((LocaleNCompare(type,"Mt16",4) == 0) ||
      (LocaleNCompare(type,"Mt32",4) == 0) ||
      (LocaleNCompare(type,"Mtrn",4) == 0)))
    {
      size=GetPSDSize(psd_info,image);
      if (size != 0)
        return(0);
      image->alpha_trait=BlendPixelTrait;
      count=ReadPSDString(image,type,4);
      if ((count != 4) || (LocaleNCompare(type,"8BIM",4) != 0))
        return(0);
      count=ReadPSDString(image,type,4);
    }
  if ((count == 4) && ((LocaleNCompare(type,"Lr16",4) == 0) ||
      (LocaleNCompare(type,"Lr32",4) == 0)))
    size=GetPSDSize(psd_info,image);
  return(size);
}