static void Unpredict16Bit(const Image *image,unsigned char *pixels,
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
      p[2]+=p[0]+((p[1]+p[3]) >> 8);
      p[3]+=p[1];
      p+=2;
    }
    p+=2;
    remaining-=row_size;
  }
}