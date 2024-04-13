static MagickBooleanType IsEMF(const unsigned char *magick,const size_t length)
{
  if (length < 48)
    return(MagickFalse);
  if (memcmp(magick+40,"\040\105\115\106\000\000\001\000",8) == 0)
    return(MagickTrue);
  return(MagickFalse);
}