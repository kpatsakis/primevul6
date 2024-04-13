static MagickBooleanType IsSIXEL(const unsigned char *magick,
  const size_t length)
{
  const unsigned char
    *end = magick + length;

  if (length < 3)
    return(MagickFalse);

  if (*magick == 0x90 || (*magick == 0x1b && *++magick == 'P')) {
    while (++magick != end) {
      if (*magick == 'q')
        return(MagickTrue);
      if (!(*magick >= '0' && *magick <= '9') && *magick != ';')
        return(MagickFalse);
    }
  }
  return(MagickFalse);
}