MagickExport void *CopyMagickMemory(void *destination,const void *source,
  const size_t size)
{
  register const unsigned char
    *p;

  register unsigned char
    *q;

  assert(destination != (void *) NULL);
  assert(source != (const void *) NULL);
  p=(const unsigned char *) source;
  q=(unsigned char *) destination;
  if (((q+size) < p) || (q > (p+size)))
    switch (size)
    {
      default: return(memcpy(destination,source,size));
      case 8: *q++=(*p++);
      case 7: *q++=(*p++);
      case 6: *q++=(*p++);
      case 5: *q++=(*p++);
      case 4: *q++=(*p++);
      case 3: *q++=(*p++);
      case 2: *q++=(*p++);
      case 1: *q++=(*p++);
      case 0: return(destination);
    }
  return(memmove(destination,source,size));
}