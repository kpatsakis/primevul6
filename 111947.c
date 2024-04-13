static StringInfo *ParseImageResourceBlocks(PSDInfo *psd_info,Image *image,
  const unsigned char *blocks,size_t length)
{
  const unsigned char
    *p;

  ssize_t
    offset;

  StringInfo
    *profile;

  unsigned char
    name_length;

  unsigned int
    count;

  unsigned short
    id,
    short_sans;

  if (length < 16)
    return((StringInfo *) NULL);
  profile=BlobToStringInfo((const unsigned char *) NULL,length);
  SetStringInfoDatum(profile,blocks);
  SetStringInfoName(profile,"8bim");
  for (p=blocks; (p >= blocks) && (p < (blocks+length-7)); )
  {
    if (LocaleNCompare((const char *) p,"8BIM",4) != 0)
      break;
    p+=4;
    p=PushShortPixel(MSBEndian,p,&id);
    p=PushCharPixel(p,&name_length);
    if ((name_length % 2) == 0)
      name_length++;
    p+=name_length;
    if (p > (blocks+length-4))
      break;
    p=PushLongPixel(MSBEndian,p,&count);
    offset=(ssize_t) count;
    if (((p+offset) < blocks) || ((p+offset) > (blocks+length)))
      break;
    switch (id)
    {
      case 0x03ed:
      {
        unsigned short
          resolution;

        /*
          Resolution info.
        */
        if (offset < 16)
          break;
        p=PushShortPixel(MSBEndian,p,&resolution);
        image->resolution.x=(double) resolution;
        (void) FormatImageProperty(image,"tiff:XResolution","%*g",
          GetMagickPrecision(),image->resolution.x);
        p=PushShortPixel(MSBEndian,p,&short_sans);
        p=PushShortPixel(MSBEndian,p,&short_sans);
        p=PushShortPixel(MSBEndian,p,&short_sans);
        p=PushShortPixel(MSBEndian,p,&resolution);
        image->resolution.y=(double) resolution;
        (void) FormatImageProperty(image,"tiff:YResolution","%*g",
          GetMagickPrecision(),image->resolution.y);
        p=PushShortPixel(MSBEndian,p,&short_sans);
        p=PushShortPixel(MSBEndian,p,&short_sans);
        p=PushShortPixel(MSBEndian,p,&short_sans);
        image->units=PixelsPerInchResolution;
        break;
      }
      case 0x0421:
      {
        if ((offset > 4) && (*(p+4) == 0))
          psd_info->has_merged_image=MagickFalse;
        p+=offset;
        break;
      }
      default:
      {
        p+=offset;
        break;
      }
    }
    if ((offset & 0x01) != 0)
      p++;
  }
  return(profile);
}