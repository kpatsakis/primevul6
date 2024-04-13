static MagickBooleanType IsPWP(const unsigned char *magick,const size_t length)
{
  if (length < 5)
    return(MagickFalse);
  if (LocaleNCompare((char *) magick,"SFW95",5) == 0)
    return(MagickTrue);
  return(MagickFalse);
}