static void Unpredict8Bit(const Image *image,unsigned char *pixels,
  const size_t count,const size_t row_size)
{
  unsigned char
    *p;

  size_t
    length,
    remaining;

  p=pixels;
  remaining=count;
  while (remaining > 0)
  {
    length=image->columns;
    while (--length)
    {
      *(p+1)+=*p;
      p++;
    }
    p++;
    remaining-=row_size;
  }
}