static MagickBooleanType IsXBM(const unsigned char *magick,const size_t length)
{
  if (length < 7)
    return(MagickFalse);
  if (memcmp(magick,"#define",7) == 0)
    return(MagickTrue);
  return(MagickFalse);
}