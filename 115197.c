Magick_png_read_raw_profile(png_struct *ping,Image *image,
   const ImageInfo *image_info, png_textp text,int ii,ExceptionInfo *exception)
{
  register ssize_t
    i;

  register unsigned char
    *dp;

  register png_charp
    sp;

  png_uint_32
    length,
    nibbles;

  StringInfo
    *profile;

  const unsigned char
    unhex[103]={0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,0,1, 2,3,4,5,6,7,8,9,0,0,
                 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,10,11,12,
                 13,14,15};

  sp=text[ii].text+1;
  /* look for newline */
  while (*sp != '\n')
     sp++;

  /* look for length */
  while (*sp == '\0' || *sp == ' ' || *sp == '\n')
     sp++;

  length=(png_uint_32) StringToLong(sp);

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
       "      length: %lu",(unsigned long) length);

  while (*sp != ' ' && *sp != '\n')
     sp++;

  /* allocate space */
  if (length == 0)
  {
    png_warning(ping,"invalid profile length");
    return(MagickFalse);
  }

  profile=BlobToStringInfo((const void *) NULL,length);

  if (profile == (StringInfo *) NULL)
  {
    png_warning(ping, "unable to copy profile");
    return(MagickFalse);
  }

  /* copy profile, skipping white space and column 1 "=" signs */
  dp=GetStringInfoDatum(profile);
  nibbles=length*2;

  for (i=0; i < (ssize_t) nibbles; i++)
  {
    while (*sp < '0' || (*sp > '9' && *sp < 'a') || *sp > 'f')
    {
      if (*sp == '\0')
        {
          png_warning(ping, "ran out of profile data");
          profile=DestroyStringInfo(profile);
          return(MagickFalse);
        }
      sp++;
    }

    if (i%2 == 0)
      *dp=(unsigned char) (16*unhex[(int) *sp++]);

    else
      (*dp++)+=unhex[(int) *sp++];
  }
  /*
    We have already read "Raw profile type.
  */
  (void) SetImageProfile(image,&text[ii].key[17],profile,exception);
  profile=DestroyStringInfo(profile);

  if (image_info->verbose)
    (void) printf(" Found a generic profile, type %s\n",&text[ii].key[17]);

  return MagickTrue;
}