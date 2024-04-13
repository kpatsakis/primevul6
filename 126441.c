static MagickBooleanType IsPICT(const unsigned char *magick,const size_t length)
{
  if (length < 12)
    return(MagickFalse);
  /*
    Embedded OLE2 macintosh have "PICT" instead of 512 platform header.
  */
  if (memcmp(magick,"PICT",4) == 0)
    return(MagickTrue);
  if (length < 528)
    return(MagickFalse);
  if (memcmp(magick+522,"\000\021\002\377\014\000",6) == 0)
    return(MagickTrue);
  return(MagickFalse);
}