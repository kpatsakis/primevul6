static inline ssize_t ReadPSDString(Image *image,char *p,const size_t length)
{
  ssize_t
    count;

  count=ReadBlob(image,length,(unsigned char *) p);
  if ((count == (ssize_t) length) && (image->endian != MSBEndian))
    {
      char
        *q;

      q=p+length;
      for(--q; p < q; ++p, --q)
      {
        *p = *p ^ *q,
        *q = *p ^ *q,
        *p = *p ^ *q;
      }
    }
  return(count);
}