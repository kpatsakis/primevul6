static MagickBooleanType IsITUFaxImage(const Image *image)
{
  const StringInfo
    *profile;

  const unsigned char
    *datum;

  profile=GetImageProfile(image,"8bim");
  if (profile == (const StringInfo *) NULL)
    return(MagickFalse);
  if (GetStringInfoLength(profile) < 5)
    return(MagickFalse);
  datum=GetStringInfoDatum(profile);
  if ((datum[0] == 0x47) && (datum[1] == 0x33) && (datum[2] == 0x46) &&
      (datum[3] == 0x41) && (datum[4] == 0x58))
    return(MagickTrue);
  return(MagickFalse);
}