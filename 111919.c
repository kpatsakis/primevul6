static void Unpredict32Bit(const Image *image,unsigned char *pixels,
  unsigned char *output_pixels,const size_t row_size)
{
  unsigned char
    *p,
    *q;

  ssize_t
    y;

  size_t
    offset1,
    offset2,
    offset3,
    remaining;

  unsigned char
    *start;

  offset1=image->columns;
  offset2=2*offset1;
  offset3=3*offset1;
  p=pixels;
  q=output_pixels;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    start=p;
    remaining=row_size;
    while (--remaining)
    {
      *(p+1)+=*p;
      p++;
    }

    p=start;
    remaining=image->columns;
    while (remaining--)
    {
      *(q++)=*p;
      *(q++)=*(p+offset1);
      *(q++)=*(p+offset2);
      *(q++)=*(p+offset3);

      p++;
    }
    p=start+row_size;
  }
}